#ifndef __X_CURVE_H__
#define __X_CURVE_H__

#pragma once

/*
===============================================================================

	Curve base template.

===============================================================================
*/

template< class type >
class xCurve {
public:
						xCurve();
	virtual				~xCurve();

	virtual int			AddValue(float time, const type &value);
	virtual void		RemoveIndex(const int index) { values.RemoveIndex(index); times.RemoveIndex(index); changed = true; }
	virtual void		Clear() { values.Clear(); times.Clear(); currentIndex = -1; changed = true; }

	virtual type		GetCurrentValue(float time) const;
	virtual type		GetCurrentFirstDerivative(float time) const;
	virtual type		GetCurrentSecondDerivative(float time) const;

	virtual bool		IsDone(float time) const;

	int					GetNumValues() const { return values.Count(); }
	void				SetValue(const int index, const type &value) { values[index] = value; changed = true; }
	type				GetValue(const int index) const { return values[index]; }
	type *				GetValueAddress(const int index) { return &values[index]; }
	float				GetTime(const int index) const { return times[index]; }

	float				GetLengthForTime(float time) const;
	float				GetTimeForLength(float length, float epsilon = 0.1f) const;
	float				GetLengthBetweenKnots(const int i0, const int i1) const;

	void				MakeUniform(float totalTime);
	void				SetConstantSpeed(float totalTime);
	void				ShiftTime(float deltaTime);
	void				Translate(const type &translation);

protected:
	xArray<float>		times;			// knots
	xArray<type>		values;			// knot values
	mutable int			currentIndex;	// cached index for fast lookup
	mutable bool		changed;		// set whenever the curve changes

	int					IndexForTime(float time) const;
	float				TimeForIndex(const int index) const;
	type				ValueForIndex(const int index) const;

	float				GetSpeed(float time) const;
	float				RombergIntegral(float t0, float t1, const int order) const;
};

/*
====================
xCurve::xCurve
====================
*/
template< class type >
X_INLINE xCurve<type>::xCurve() {
	currentIndex = -1;
	changed = false;
}

/*
====================
xCurve::~xCurve
====================
*/
template< class type >
X_INLINE xCurve<type>::~xCurve() {
}

/*
====================
xCurve::AddValue

  add a timed/value pair to the spline
  returns the index to the inserted pair
====================
*/
template< class type >
X_INLINE int xCurve<type>::AddValue(float time, const type &value) {
	int i;

	i = IndexForTime(time);
	times.Insert(time, i);
	values.Insert(value, i);
	changed = true;
	return i;
}

/*
====================
xCurve::GetCurrentValue

  get the value for the given time
====================
*/
template< class type >
X_INLINE type xCurve<type>::GetCurrentValue(float time) const {
	int i;

	i = IndexForTime(time);
	if (i >= values.Count()) {
		return values[values.Count() - 1];
	} else {
		return values[i];
	}
}

/*
====================
xCurve::GetCurrentFirstDerivative

  get the first derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve<type>::GetCurrentFirstDerivative(float time) const {
	return (values[0] - values[0]);
}

/*
====================
xCurve::GetCurrentSecondDerivative

  get the second derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve<type>::GetCurrentSecondDerivative(float time) const {
	return (values[0] - values[0]);
}

/*
====================
xCurve::IsDone
====================
*/
template< class type >
X_INLINE bool xCurve<type>::IsDone(float time) const {
	return (time >= times[ times.Count() - 1 ]);
}

/*
====================
xCurve::GetSpeed
====================
*/
template< class type >
X_INLINE float xCurve<type>::GetSpeed(float time) const {
	int i;
	float speed;
	type value;

	value = GetCurrentFirstDerivative(time);
	for (speed = 0.0f, i = 0; i < value.Dimension(); i++) {
		speed += value[i] * value[i];
	}
	return xMath::Sqrt(speed);
}

/*
====================
xCurve::RombergIntegral
====================
*/
template< class type >
X_INLINE float xCurve<type>::RombergIntegral(float t0, float t1, const int order) const {
	int i, j, k, m, n;
	float sum, delta;
	float *temp[2];

	temp[0] = (float *) _alloca16(order * sizeof(float));
	temp[1] = (float *) _alloca16(order * sizeof(float));

	delta = t1 - t0;
	temp[0][0] = 0.5f * delta * (GetSpeed(t0) + GetSpeed(t1));

	for (i = 2, m = 1; i <= order; i++, m *= 2, delta *= 0.5f) {

		// approximate using the trapezoid rule
		sum = 0.0f;
		for (j = 1; j <= m; j++) {
			sum += GetSpeed(t0 + delta * (j - 0.5f));
		}

		// Richardson extrapolation
		temp[1][0] = 0.5f * (temp[0][0] + delta * sum);
		for (k = 1, n = 4; k < i; k++, n *= 4) {
			temp[1][k] = (n * temp[1][k-1] - temp[0][k-1]) / (n - 1);
		}

		for (j = 0; j < i; j++) {
			temp[0][j] = temp[1][j];
		}
	}
	return temp[0][order-1];
}

/*
====================
xCurve::GetLengthBetweenKnots
====================
*/
template< class type >
X_INLINE float xCurve<type>::GetLengthBetweenKnots(const int i0, const int i1) const {
	float length = 0.0f;
	for (int i = i0; i < i1; i++) {
		length += RombergIntegral(times[i], times[i+1], 5);
	}
	return length;
}

/*
====================
xCurve::GetLengthForTime
====================
*/
template< class type >
X_INLINE float xCurve<type>::GetLengthForTime(float time) const {
	float length = 0.0f;
	int index = IndexForTime(time);
	for (int i = 0; i < index; i++) {
		length += RombergIntegral(times[i], times[i+1], 5);
	}
	length += RombergIntegral(times[index], time, 5);
	return length;
}

/*
====================
xCurve::GetTimeForLength
====================
*/
template< class type >
X_INLINE float xCurve<type>::GetTimeForLength(float length, float epsilon) const {
	int i, index;
	float *accumLength, totalLength, len0, len1, t, diff;

	if (length <= 0.0f) {
		return times[0];
	}

	accumLength = (float *) _alloca16(values.Count() * sizeof(float));
	totalLength = 0.0f;
	for (index = 0; index < values.Count() - 1; index++) {
		totalLength += GetLengthBetweenKnots(index, index + 1);
		accumLength[index] = totalLength;
		if (length < accumLength[index]) {
			break;
		}
	}

	if (index >= values.Count() - 1) {
		return times[times.Count() - 1];
	}

	if (index == 0) {
		len0 = length;
		len1 = accumLength[0];
	} else {
		len0 = length - accumLength[index-1];
		len1 = accumLength[index] - accumLength[index-1];
	}

	// invert the arc length integral using Newton's method
	t = (times[index+1] - times[index]) * len0 / len1;
	for (i = 0; i < 32; i++) {
		diff = RombergIntegral(times[index], times[index] + t, 5) - len0;
		if (xMath::Fabs(diff) <= epsilon) {
			return times[index] + t;
		}
		t -= diff / GetSpeed(times[index] + t);
	}
	return times[index] + t;
}

/*
====================
xCurve::MakeUniform
====================
*/
template< class type >
X_INLINE void xCurve<type>::MakeUniform(float totalTime) {
	int i, n;

	n = times.Count() - 1;
	for (i = 0; i <= n; i++) {
		times[i] = i * totalTime / n;
	}
	changed = true;
}

/*
====================
xCurve::SetConstantSpeed
====================
*/
template< class type >
X_INLINE void xCurve<type>::SetConstantSpeed(float totalTime) {
	int i, j;
	float *length, totalLength, scale, t;

	length = (float *) _alloca16(values.Count() * sizeof(float));
	totalLength = 0.0f;
	for (i = 0; i < values.Count() - 1; i++) {
		length[i] = GetLengthBetweenKnots(i, i + 1);
		totalLength += length[i];
	}
	scale = totalTime / totalLength;
	for (t = 0.0f, i = 0; i < times.Count() - 1; i++) {
		times[i] = t;
		t += scale * length[i];
	}
	times[times.Count() - 1] = totalTime;
	changed = true;
}

/*
====================
xCurve::ShiftTime
====================
*/
template< class type >
X_INLINE void xCurve<type>::ShiftTime(float deltaTime) {
	for (int i = 0; i < times.Count(); i++) {
		times[i] += deltaTime;
	}
	changed = true;
}

/*
====================
xCurve::Translate
====================
*/
template< class type >
X_INLINE void xCurve<type>::Translate(const type &translation) {
	for (int i = 0; i < values.Count(); i++) {
		values[i] += translation;
	}
	changed = true;
}

/*
====================
xCurve::IndexForTime

  find the index for the first time greater than or equal to the given time
====================
*/
template< class type >
X_INLINE int xCurve<type>::IndexForTime(float time) const {
	int len, mid, offset, res;

	if (currentIndex >= 0 && currentIndex <= times.Count()) {
		// use the cached index if it is still valid
		if (currentIndex == 0) {
			if (time <= times[currentIndex]) {
				return currentIndex;
			}
		} else if (currentIndex == times.Count()) {
			if (time > times[currentIndex-1]) {
				return currentIndex;
			}
		} else if (time > times[currentIndex-1] && time <= times[currentIndex]) {
			return currentIndex;
		} else if (time > times[currentIndex] && (currentIndex+1 == times.Count() || time <= times[currentIndex+1])) {
			// use the next index
			currentIndex++;
			return currentIndex;
		}
	}

	// use binary search to find the index for the given time
	len = times.Count();
	mid = len;
	offset = 0;
	res = 0;
	while(mid > 0) {
		mid = len >> 1;
		if (time == times[offset+mid]) {
			return offset+mid;
		} else if (time > times[offset+mid]) {
			offset += mid;
			len -= mid;
			res = 1;
		} else {
			len -= mid;
			res = 0;
		}
	}
	currentIndex = offset+res;
	return currentIndex;
}

/*
====================
xCurve::ValueForIndex

  get the value for the given time
====================
*/
template< class type >
X_INLINE type xCurve<type>::ValueForIndex(const int index) const {
	int n = values.Count()-1;

	if (index < 0) {
		return values[0] + index * (values[1] - values[0]);
	} else if (index > n) {
		return values[n] + (index - n) * (values[n] - values[n-1]);
	}
	return values[index];
}

/*
====================
xCurve::TimeForIndex

  get the value for the given time
====================
*/
template< class type >
X_INLINE float xCurve<type>::TimeForIndex(const int index) const {
	int n = times.Count()-1;

	if (index < 0) {
		return times[0] + index * (times[1] - times[0]);
	} else if (index > n) {
		return times[n] + (index - n) * (times[n] - times[n-1]);
	}
	return times[index];
}


/*
===============================================================================

	Bezier Curve template.
	The degree of the polynomial equals the number of knots minus one.

===============================================================================
*/

template< class type >
class xCurve_Bezier : public xCurve<type> {
public:
						xCurve_Bezier();

	virtual type		GetCurrentValue(float time) const;
	virtual type		GetCurrentFirstDerivative(float time) const;
	virtual type		GetCurrentSecondDerivative(float time) const;

protected:
	void				Basis(const int order, float t, float *bvals) const;
	void				BasisFirstDerivative(const int order, float t, float *bvals) const;
	void				BasisSecondDerivative(const int order, float t, float *bvals) const;
};

/*
====================
xCurve_Bezier::xCurve_Bezier
====================
*/
template< class type >
X_INLINE xCurve_Bezier<type>::xCurve_Bezier() {
}

/*
====================
xCurve_Bezier::GetCurrentValue

  get the value for the given time
====================
*/
template< class type >
X_INLINE type xCurve_Bezier<type>::GetCurrentValue(float time) const {
	int i;
	float *bvals;
	type v;

	bvals = (float *) _alloca16(values.Count() * sizeof(float));

	Basis(values.Count(), time, bvals);
	v = bvals[0] * values[0];
	for (i = 1; i < values.Count(); i++) {
		v += bvals[i] * values[i];
	}
	return v;
}

/*
====================
xCurve_Bezier::GetCurrentFirstDerivative

  get the first derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve_Bezier<type>::GetCurrentFirstDerivative(float time) const {
	int i;
	float *bvals, d;
	type v;

	bvals = (float *) _alloca16(values.Count() * sizeof(float));

	BasisFirstDerivative(values.Count(), time, bvals);
	v = bvals[0] * values[0];
	for (i = 1; i < values.Count(); i++) {
		v += bvals[i] * values[i];
	}
	d = (times[times.Count()-1] - times[0]);
	return ((float) (values.Count()-1) / d) * v;
}

/*
====================
xCurve_Bezier::GetCurrentSecondDerivative

  get the second derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve_Bezier<type>::GetCurrentSecondDerivative(float time) const {
	int i;
	float *bvals, d;
	type v;

	bvals = (float *) _alloca16(values.Count() * sizeof(float));

	BasisSecondDerivative(values.Count(), time, bvals);
	v = bvals[0] * values[0];
	for (i = 1; i < values.Count(); i++) {
		v += bvals[i] * values[i];
	}
	d = (times[times.Count()-1] - times[0]);
	return ((float) (values.Count()-2) * (values.Count()-1) / (d * d)) * v;
}

/*
====================
xCurve_Bezier::Basis

  bezier basis functions
====================
*/
template< class type >
X_INLINE void xCurve_Bezier<type>::Basis(const int order, float t, float *bvals) const {
	int i, j, d;
	float *c, c1, c2, s, o, ps, po;

	bvals[0] = 1.0f;
	d = order - 1;
	if (d <= 0) {
		return;
	}

	c = (float *) _alloca16((d+1) * sizeof(float));
	s = (float) (t - times[0]) / (times[times.Count()-1] - times[0]);
    o = 1.0f - s;
	ps = s;
	po = o;

	for (i = 1; i < d; i++) {
		c[i] = 1.0f;
	}
	for (i = 1; i < d; i++) {
		c[i-1] = 0.0f;
		c1 = c[i];
		c[i] = 1.0f;
		for (j = i+1; j <= d; j++) {
			c2 = c[j];
			c[j] = c1 + c[j-1];
			c1 = c2;
		}
		bvals[i] = c[d] * ps;
		ps *= s;
	}
	for (i = d-1; i >= 0; i--) {
		bvals[i] *= po;
		po *= o;
	}
	bvals[d] = ps;
}

/*
====================
xCurve_Bezier::BasisFirstDerivative

  first derivative of bezier basis functions
====================
*/
template< class type >
X_INLINE void xCurve_Bezier<type>::BasisFirstDerivative(const int order, float t, float *bvals) const {
	int i;

	Basis(order-1, t, bvals+1);
	bvals[0] = 0.0f;
	for (i = 0; i < order-1; i++) {
		bvals[i] -= bvals[i+1];
	}
}

/*
====================
xCurve_Bezier::BasisSecondDerivative

  second derivative of bezier basis functions
====================
*/
template< class type >
X_INLINE void xCurve_Bezier<type>::BasisSecondDerivative(const int order, float t, float *bvals) const {
	int i;

	BasisFirstDerivative(order-1, t, bvals+1);
	bvals[0] = 0.0f;
	for (i = 0; i < order-1; i++) {
		bvals[i] -= bvals[i+1];
	}
}


/*
===============================================================================

	Quadratic Bezier Curve template.
	Should always have exactly three knots.

===============================================================================
*/

template< class type >
class xCurve_QuadraticBezier : public xCurve<type> {
public:
						xCurve_QuadraticBezier();

	virtual type		GetCurrentValue(float time) const;
	virtual type		GetCurrentFirstDerivative(float time) const;
	virtual type		GetCurrentSecondDerivative(float time) const;

protected:
	void				Basis(float t, float *bvals) const;
	void				BasisFirstDerivative(float t, float *bvals) const;
	void				BasisSecondDerivative(float t, float *bvals) const;
};

/*
====================
xCurve_QuadraticBezier::xCurve_QuadraticBezier
====================
*/
template< class type >
X_INLINE xCurve_QuadraticBezier<type>::xCurve_QuadraticBezier() {
}


/*
====================
xCurve_QuadraticBezier::GetCurrentValue

  get the value for the given time
====================
*/
template< class type >
X_INLINE type xCurve_QuadraticBezier<type>::GetCurrentValue(float time) const {
	float bvals[3];
	assert(values.Count() == 3);
	Basis(time, bvals);
	return (bvals[0] * values[0] + bvals[1] * values[1] + bvals[2] * values[2]);
}

/*
====================
xCurve_QuadraticBezier::GetCurrentFirstDerivative

  get the first derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve_QuadraticBezier<type>::GetCurrentFirstDerivative(float time) const {
	float bvals[3], d;
	assert(values.Count() == 3);
	BasisFirstDerivative(time, bvals);
	d = (times[2] - times[0]);
	return (bvals[0] * values[0] + bvals[1] * values[1] + bvals[2] * values[2]) / d;
}

/*
====================
xCurve_QuadraticBezier::GetCurrentSecondDerivative

  get the second derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve_QuadraticBezier<type>::GetCurrentSecondDerivative(float time) const {
	float bvals[3], d;
	assert(values.Count() == 3);
	BasisSecondDerivative(time, bvals);
	d = (times[2] - times[0]);
	return (bvals[0] * values[0] + bvals[1] * values[1] + bvals[2] * values[2]) / (d * d);
}

/*
====================
xCurve_QuadraticBezier::Basis

  quadratic bezier basis functions
====================
*/
template< class type >
X_INLINE void xCurve_QuadraticBezier<type>::Basis(float t, float *bvals) const {
	float s1 = (float) (t - times[0]) / (times[2] - times[0]);
	float s2 = s1 * s1;
	bvals[0] = s2 - 2.0f * s1 + 1.0f;
	bvals[1] = -2.0f * s2 + 2.0f * s1;
	bvals[2] = s2;
}

/*
====================
xCurve_QuadraticBezier::BasisFirstDerivative

  first derivative of quadratic bezier basis functions
====================
*/
template< class type >
X_INLINE void xCurve_QuadraticBezier<type>::BasisFirstDerivative(float t, float *bvals) const {
	float s1 = (float) (t - times[0]) / (times[2] - times[0]);
	bvals[0] = 2.0f * s1 - 2.0f;
	bvals[1] = -4.0f * s1 + 2.0f;
	bvals[2] = 2.0f * s1;
}

/*
====================
xCurve_QuadraticBezier::BasisSecondDerivative

  second derivative of quadratic bezier basis functions
====================
*/
template< class type >
X_INLINE void xCurve_QuadraticBezier<type>::BasisSecondDerivative(float t, float *bvals) const {
	float s1 = (float) (t - times[0]) / (times[2] - times[0]);
	bvals[0] = 2.0f;
	bvals[1] = -4.0f;
	bvals[2] = 2.0f;
}


/*
===============================================================================

	Cubic Bezier Curve template.
	Should always have exactly four knots.

===============================================================================
*/

template< class type >
class xCurve_CubicBezier : public xCurve<type> {
public:
						xCurve_CubicBezier();

	virtual type		GetCurrentValue(float time) const;
	virtual type		GetCurrentFirstDerivative(float time) const;
	virtual type		GetCurrentSecondDerivative(float time) const;

protected:
	void				Basis(float t, float *bvals) const;
	void				BasisFirstDerivative(float t, float *bvals) const;
	void				BasisSecondDerivative(float t, float *bvals) const;
};

/*
====================
xCurve_CubicBezier::xCurve_CubicBezier
====================
*/
template< class type >
X_INLINE xCurve_CubicBezier<type>::xCurve_CubicBezier() {
}


/*
====================
xCurve_CubicBezier::GetCurrentValue

  get the value for the given time
====================
*/
template< class type >
X_INLINE type xCurve_CubicBezier<type>::GetCurrentValue(float time) const {
	float bvals[4];
	assert(values.Count() == 4);
	Basis(time, bvals);
	return (bvals[0] * values[0] + bvals[1] * values[1] + bvals[2] * values[2] + bvals[3] * values[3]);
}

/*
====================
xCurve_CubicBezier::GetCurrentFirstDerivative

  get the first derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve_CubicBezier<type>::GetCurrentFirstDerivative(float time) const {
	float bvals[4], d;
	assert(values.Count() == 4);
	BasisFirstDerivative(time, bvals);
	d = (times[3] - times[0]);
	return (bvals[0] * values[0] + bvals[1] * values[1] + bvals[2] * values[2] + bvals[3] * values[3]) / d;
}

/*
====================
xCurve_CubicBezier::GetCurrentSecondDerivative

  get the second derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve_CubicBezier<type>::GetCurrentSecondDerivative(float time) const {
	float bvals[4], d;
	assert(values.Count() == 4);
	BasisSecondDerivative(time, bvals);
	d = (times[3] - times[0]);
	return (bvals[0] * values[0] + bvals[1] * values[1] + bvals[2] * values[2] + bvals[3] * values[3]) / (d * d);
}

/*
====================
xCurve_CubicBezier::Basis

  cubic bezier basis functions
====================
*/
template< class type >
X_INLINE void xCurve_CubicBezier<type>::Basis(float t, float *bvals) const {
	float s1 = (float) (t - times[0]) / (times[3] - times[0]);
	float s2 = s1 * s1;
	float s3 = s2 * s1;
	bvals[0] = -s3 + 3.0f * s2 - 3.0f * s1 + 1.0f;
	bvals[1] = 3.0f * s3 - 6.0f * s2 + 3.0f * s1;
	bvals[2] = -3.0f * s3 + 3.0f * s2;
	bvals[3] = s3;
}

/*
====================
xCurve_CubicBezier::BasisFirstDerivative

  first derivative of cubic bezier basis functions
====================
*/
template< class type >
X_INLINE void xCurve_CubicBezier<type>::BasisFirstDerivative(float t, float *bvals) const {
	float s1 = (float) (t - times[0]) / (times[3] - times[0]);
	float s2 = s1 * s1;
	bvals[0] = -3.0f * s2 + 6.0f * s1 - 3.0f;
	bvals[1] = 9.0f * s2 - 12.0f * s1 + 3.0f;
	bvals[2] = -9.0f * s2 + 6.0f * s1;
	bvals[3] = 3.0f * s2;
}

/*
====================
xCurve_CubicBezier::BasisSecondDerivative

  second derivative of cubic bezier basis functions
====================
*/
template< class type >
X_INLINE void xCurve_CubicBezier<type>::BasisSecondDerivative(float t, float *bvals) const {
	float s1 = (float) (t - times[0]) / (times[3] - times[0]);
	bvals[0] = -6.0f * s1 + 6.0f;
	bvals[1] = 18.0f * s1 - 12.0f;
	bvals[2] = -18.0f * s1 + 6.0f;
	bvals[3] = 6.0f * s1;
}


/*
===============================================================================

	Spline base template.

===============================================================================
*/

template< class type >
class xCurve_Spline : public xCurve<type> {
public:
	enum				boundary_t { BT_FREE, BT_CLAMPED, BT_CLOSED };

						xCurve_Spline();

	virtual bool		IsDone(float time) const;

	virtual void		SetBoundaryType(const boundary_t bt) { boundaryType = bt; changed = true; }
	virtual boundary_t	GetBoundaryType() const { return boundaryType; }

	virtual void		SetCloseTime(float t) { closeTime = t; changed = true; }
	virtual float		GetCloseTime() const { return boundaryType == BT_CLOSED ? closeTime : 0.0f; }

protected:
	boundary_t			boundaryType;
	float				closeTime;

	type				ValueForIndex(const int index) const;
	float				TimeForIndex(const int index) const;
	float				ClampedTime(float t) const;
};

/*
====================
xCurve_Spline::xCurve_Spline
====================
*/
template< class type >
X_INLINE xCurve_Spline<type>::xCurve_Spline() {
	boundaryType = BT_FREE;
	closeTime = 0.0f;
}

/*
====================
xCurve_Spline::ValueForIndex

  get the value for the given time
====================
*/
template< class type >
X_INLINE type xCurve_Spline<type>::ValueForIndex(const int index) const {
	int n = values.Count()-1;

	if (index < 0) {
		if (boundaryType == BT_CLOSED) {
			return values[ values.Count() + index % values.Count() ];
		}
		else {
			return values[0] + index * (values[1] - values[0]);
		}
	}
	else if (index > n) {
		if (boundaryType == BT_CLOSED) {
			return values[ index % values.Count() ];
		}
		else {
			return values[n] + (index - n) * (values[n] - values[n-1]);
		}
	}
	return values[index];
}

/*
====================
xCurve_Spline::TimeForIndex

  get the value for the given time
====================
*/
template< class type >
X_INLINE float xCurve_Spline<type>::TimeForIndex(const int index) const {
	int n = times.Count()-1;

	if (index < 0) {
		if (boundaryType == BT_CLOSED) {
			return (index / times.Count()) * (times[n] + closeTime) - (times[n] + closeTime - times[times.Count() + index % times.Count()]);
		}
		else {
			return times[0] + index * (times[1] - times[0]);
		}
	}
	else if (index > n) {
		if (boundaryType == BT_CLOSED) {
			return (index / times.Count()) * (times[n] + closeTime) + times[index % times.Count()];
		}
		else {
			return times[n] + (index - n) * (times[n] - times[n-1]);
		}
	}
	return times[index];
}

/*
====================
xCurve_Spline::ClampedTime

  return the clamped time based on the boundary type
====================
*/
template< class type >
X_INLINE float xCurve_Spline<type>::ClampedTime(float t) const {
	if (boundaryType == BT_CLAMPED) {
		if (t < times[0]) {
			return times[0];
		}
		else if (t >= times[times.Count()-1]) {
			return times[times.Count()-1];
		}
	}
	return t;
}

/*
====================
xCurve_Spline::IsDone
====================
*/
template< class type >
X_INLINE bool xCurve_Spline<type>::IsDone(float time) const {
	return (boundaryType != BT_CLOSED && time >= times[ times.Count() - 1 ]);
}


/*
===============================================================================

	Cubic Interpolating Spline template.
	The curve goes through all the knots.

===============================================================================
*/

template< class type >
class xCurve_NaturalCubicSpline : public xCurve_Spline<type> {
public:
						xCurve_NaturalCubicSpline();

	virtual void		Clear() { xCurve_Spline<type>::Clear(); values.Clear(); b.Clear(); c.Clear(); d.Clear(); }

	virtual type		GetCurrentValue(float time) const;
	virtual type		GetCurrentFirstDerivative(float time) const;
	virtual type		GetCurrentSecondDerivative(float time) const;

protected:
	mutable xArray<type>b;
	mutable xArray<type>c;
	mutable xArray<type>d;

	void				Setup() const;
	void				SetupFree() const;
	void				SetupClamped() const;
	void				SetupClosed() const;
};

/*
====================
xCurve_NaturalCubicSpline::xCurve_NaturalCubicSpline
====================
*/
template< class type >
X_INLINE xCurve_NaturalCubicSpline<type>::xCurve_NaturalCubicSpline() {
}

/*
====================
xCurve_NaturalCubicSpline::GetCurrentValue

  get the value for the given time
====================
*/
template< class type >
X_INLINE type xCurve_NaturalCubicSpline<type>::GetCurrentValue(float time) const {
	float clampedTime = ClampedTime(time);
	int i = IndexForTime(clampedTime);
	float s = time - TimeForIndex(i);
	Setup();
	return (values[i] + s * (b[i] + s * (c[i] + s * d[i])));
}

/*
====================
xCurve_NaturalCubicSpline::GetCurrentFirstDerivative

  get the first derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve_NaturalCubicSpline<type>::GetCurrentFirstDerivative(float time) const {
	float clampedTime = ClampedTime(time);
	int i = IndexForTime(clampedTime);
	float s = time - TimeForIndex(i);
	Setup();
	return (b[i] + s * (2.0f * c[i] + 3.0f * s * d[i]));
}

/*
====================
xCurve_NaturalCubicSpline::GetCurrentSecondDerivative

  get the second derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve_NaturalCubicSpline<type>::GetCurrentSecondDerivative(float time) const {
	float clampedTime = ClampedTime(time);
	int i = IndexForTime(clampedTime);
	float s = time - TimeForIndex(i);
	Setup();
	return (2.0f * c[i] + 6.0f * s * d[i]);
}

/*
====================
xCurve_NaturalCubicSpline::Setup
====================
*/
template< class type >
X_INLINE void xCurve_NaturalCubicSpline<type>::Setup() const {
	if (changed) {
		switch(boundaryType) {
			case BT_FREE:		SetupFree(); break;
			case BT_CLAMPED:	SetupClamped(); break;
			case BT_CLOSED:		SetupClosed(); break;
		}
		changed = false;
	}
}

/*
====================
xCurve_NaturalCubicSpline::SetupFree
====================
*/
template< class type >
X_INLINE void xCurve_NaturalCubicSpline<type>::SetupFree() const {
	int i;
	float inv;
	float *d0, *d1, *beta, *gamma;
	type *alpha, *delta;

	d0 = (float *) _alloca16((values.Count() - 1) * sizeof(float));
	d1 = (float *) _alloca16((values.Count() - 1) * sizeof(float));
	alpha = (type *) _alloca16((values.Count() - 1) * sizeof(type));
	beta = (float *) _alloca16(values.Count() * sizeof(float));
	gamma = (float *) _alloca16((values.Count() - 1) * sizeof(float));
	delta = (type *) _alloca16(values.Count() * sizeof(type));

	for (i = 0; i < values.Count() - 1; i++) {
		d0[i] = times[i+1] - times[i];
	}

	for (i = 1; i < values.Count() - 1; i++) {
		d1[i] = times[i+1] - times[i-1];
	}

	for (i = 1; i < values.Count() - 1; i++) {
		type sum = 3.0f * (d0[i-1] * values[i+1] - d1[i] * values[i] + d0[i] * values[i-1]);
		inv = 1.0f / (d0[i-1] * d0[i]);
		alpha[i] = inv * sum;
	}

	beta[0] = 1.0f;
	gamma[0] = 0.0f;
	delta[0] = values[0] - values[0];

	for (i = 1; i < values.Count() - 1; i++) {
		beta[i] = 2.0f * d1[i] - d0[i-1] * gamma[i-1];
		inv = 1.0f / beta[i];
		gamma[i] = inv * d0[i];
		delta[i] = inv * (alpha[i] - d0[i-1] * delta[i-1]);
	}
	beta[values.Count() - 1] = 1.0f;
	delta[values.Count() - 1] = values[0] - values[0];

	b.AssureSize(values.Count());
	c.AssureSize(values.Count());
	d.AssureSize(values.Count());

	c[values.Count() - 1] = values[0] - values[0];

	for (i = values.Count() - 2; i >= 0; i--) {
		c[i] = delta[i] - gamma[i] * c[i+1];
		inv = 1.0f / d0[i];
		b[i] = inv * (values[i+1] - values[i]) - (1.0f / 3.0f) * d0[i] * (c[i+1] + 2.0f * c[i]);
		d[i] = (1.0f / 3.0f) * inv * (c[i+1] - c[i]);
	}
}

/*
====================
xCurve_NaturalCubicSpline::SetupClamped
====================
*/
template< class type >
X_INLINE void xCurve_NaturalCubicSpline<type>::SetupClamped() const {
	int i;
	float inv;
	float *d0, *d1, *beta, *gamma;
	type *alpha, *delta;

	d0 = (float *) _alloca16((values.Count() - 1) * sizeof(float));
	d1 = (float *) _alloca16((values.Count() - 1) * sizeof(float));
	alpha = (type *) _alloca16((values.Count() - 1) * sizeof(type));
	beta = (float *) _alloca16(values.Count() * sizeof(float));
	gamma = (float *) _alloca16((values.Count() - 1) * sizeof(float));
	delta = (type *) _alloca16(values.Count() * sizeof(type));

	for (i = 0; i < values.Count() - 1; i++) {
		d0[i] = times[i+1] - times[i];
	}

	for (i = 1; i < values.Count() - 1; i++) {
		d1[i] = times[i+1] - times[i-1];
	}

	inv = 1.0f / d0[0];
	alpha[0] = 3.0f * (inv - 1.0f) * (values[1] - values[0]);
	inv = 1.0f / d0[values.Count() - 2];
	alpha[values.Count() - 1] = 3.0f * (1.0f - inv) * (values[values.Count() - 1] - values[values.Count() - 2]);

	for (i = 1; i < values.Count() - 1; i++) {
		type sum = 3.0f * (d0[i-1] * values[i+1] - d1[i] * values[i] + d0[i] * values[i-1]);
		inv = 1.0f / (d0[i-1] * d0[i]);
		alpha[i] = inv * sum;
	}

	beta[0] = 2.0f * d0[0];
	gamma[0] = 0.5f;
	inv = 1.0f / beta[0];
	delta[0] = inv * alpha[0];

	for (i = 1; i < values.Count() - 1; i++) {
		beta[i] = 2.0f * d1[i] - d0[i-1] * gamma[i-1];
		inv = 1.0f / beta[i];
		gamma[i] = inv * d0[i];
		delta[i] = inv * (alpha[i] - d0[i-1] * delta[i-1]);
	}

	beta[values.Count() - 1] = d0[values.Count() - 2] * (2.0f - gamma[values.Count() - 2]);
	inv = 1.0f / beta[values.Count() - 1];
	delta[values.Count() - 1] = inv * (alpha[values.Count() - 1] - d0[values.Count() - 2] * delta[values.Count() - 2]);

	b.AssureSize(values.Count());
	c.AssureSize(values.Count());
	d.AssureSize(values.Count());

	c[values.Count() - 1] = delta[values.Count() - 1];

	for (i = values.Count() - 2; i >= 0; i--) {
		c[i] = delta[i] - gamma[i] * c[i+1];
		inv = 1.0f / d0[i];
		b[i] = inv * (values[i+1] - values[i]) - (1.0f / 3.0f) * d0[i]* (c[i+1] + 2.0f * c[i]);
		d[i] = (1.0f / 3.0f) * inv * (c[i+1] - c[i]);
	}
}

/*
====================
xCurve_NaturalCubicSpline::SetupClosed
====================
*/
template< class type >
X_INLINE void xCurve_NaturalCubicSpline<type>::SetupClosed() const {
	int i, j;
	float c0, c1;
	float *d0;
	xMatX mat;
	xVecX x;

	d0 = (float *) _alloca16((values.Count() - 1) * sizeof(float));
	x.SetData(values.Count(), VECX_ALLOCA(values.Count()));
	mat.SetData(values.Count(), values.Count(), MATX_ALLOCA(values.Count() * values.Count()));

	b.AssureSize(values.Count());
	c.AssureSize(values.Count());
	d.AssureSize(values.Count());

	for (i = 0; i < values.Count() - 1; i++) {
		d0[i] = times[i+1] - times[i];
	}

	// matrix of system
	mat[0][0] = 1.0f;
	mat[0][values.Count() - 1] = -1.0f;
	for (i = 1; i <= values.Count() - 2; i++) {
		mat[i][i-1] = d0[i-1];
		mat[i][i  ] = 2.0f * (d0[i-1] + d0[i]);
		mat[i][i+1] = d0[i];
	}
	mat[values.Count() - 1][values.Count() - 2] = d0[values.Count() - 2];
	mat[values.Count() - 1][0] = 2.0f * (d0[values.Count() - 2] + d0[0]);
	mat[values.Count() - 1][1] = d0[0];

	// right-hand side
	c[0].Zero();
	for (i = 1; i <= values.Count() - 2; i++) {
		c0 = 1.0f / d0[i];
		c1 = 1.0f / d0[i-1];
		c[i] = 3.0f * (c0 * (values[i + 1] - values[i]) - c1 * (values[i] - values[i - 1]));
	}
	c0 = 1.0f / d0[0];
	c1 = 1.0f / d0[values.Count() - 2];
	c[values.Count() - 1] = 3.0f * (c0 * (values[1] - values[0]) - c1 * (values[0] - values[values.Count() - 2]));

	// solve system for each dimension
	mat.LU_Factor(NULL);
	for (i = 0; i < values[0].Dimension(); i++) {
		for (j = 0; j < values.Count(); j++) {
			x[j] = c[j][i];
		}
		mat.LU_Solve(x, x, NULL);
		for (j = 0; j < values.Count(); j++) {
			c[j][i] = x[j];
		}
	}

	for (i = 0; i < values.Count() - 1; i++) {
		c0 = 1.0f / d0[i];
		b[i] = c0 * (values[i + 1] - values[i]) - (1.0f / 3.0f) * (c[i+1] + 2.0f * c[i]) * d0[i];
		d[i] = (1.0f / 3.0f) * c0 * (c[i + 1] - c[i]);
	}
}


/*
===============================================================================

	Uniform Cubic Interpolating Spline template.
	The curve goes through all the knots.

===============================================================================
*/

template< class type >
class xCurve_CatmullRomSpline : public xCurve_Spline<type> {
public:
						xCurve_CatmullRomSpline();

	virtual type		GetCurrentValue(float time) const;
	virtual type		GetCurrentFirstDerivative(float time) const;
	virtual type		GetCurrentSecondDerivative(float time) const;

protected:
	void				Basis(const int index, float t, float *bvals) const;
	void				BasisFirstDerivative(const int index, float t, float *bvals) const;
	void				BasisSecondDerivative(const int index, float t, float *bvals) const;
};

/*
====================
xCurve_CatmullRomSpline::xCurve_CatmullRomSpline
====================
*/
template< class type >
X_INLINE xCurve_CatmullRomSpline<type>::xCurve_CatmullRomSpline() {
}

/*
====================
xCurve_CatmullRomSpline::GetCurrentValue

  get the value for the given time
====================
*/
template< class type >
X_INLINE type xCurve_CatmullRomSpline<type>::GetCurrentValue(float time) const {
	int i, j, k;
	float bvals[4], clampedTime;
	type v;

	if (times.Count() == 1) {
		return values[0];
	}

	clampedTime = ClampedTime(time);
	i = IndexForTime(clampedTime);
	Basis(i-1, clampedTime, bvals);
	v = values[0] - values[0];
	for (j = 0; j < 4; j++) {
		k = i + j - 2;
		v += bvals[j] * ValueForIndex(k);
	}
	return v;
}

/*
====================
xCurve_CatmullRomSpline::GetCurrentFirstDerivative

  get the first derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve_CatmullRomSpline<type>::GetCurrentFirstDerivative(float time) const {
	int i, j, k;
	float bvals[4], d, clampedTime;
	type v;

	if (times.Count() == 1) {
		return (values[0] - values[0]);
	}

	clampedTime = ClampedTime(time);
	i = IndexForTime(clampedTime);
	BasisFirstDerivative(i-1, clampedTime, bvals);
	v = values[0] - values[0];
	for (j = 0; j < 4; j++) {
		k = i + j - 2;
		v += bvals[j] * ValueForIndex(k);
	}
	d = (TimeForIndex(i) - TimeForIndex(i-1));
	return v / d;
}

/*
====================
xCurve_CatmullRomSpline::GetCurrentSecondDerivative

  get the second derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve_CatmullRomSpline<type>::GetCurrentSecondDerivative(float time) const {
	int i, j, k;
	float bvals[4], d, clampedTime;
	type v;

	if (times.Count() == 1) {
		return (values[0] - values[0]);
	}

	clampedTime = ClampedTime(time);
	i = IndexForTime(clampedTime);
	BasisSecondDerivative(i-1, clampedTime, bvals);
	v = values[0] - values[0];
	for (j = 0; j < 4; j++) {
		k = i + j - 2;
		v += bvals[j] * ValueForIndex(k);
	}
	d = (TimeForIndex(i) - TimeForIndex(i-1));
	return v / (d * d);
}

/*
====================
xCurve_CatmullRomSpline::Basis

  spline basis functions
====================
*/
template< class type >
X_INLINE void xCurve_CatmullRomSpline<type>::Basis(const int index, float t, float *bvals) const {
	float s = (float) (t - TimeForIndex(index)) / (TimeForIndex(index+1) - TimeForIndex(index));
	bvals[0] = ((-s + 2.0f) * s - 1.0f) * s * 0.5f;				// -0.5f s * s * s + s * s - 0.5f * s
	bvals[1] = (((3.0f * s - 5.0f) * s) * s + 2.0f) * 0.5f;	// 1.5f * s * s * s - 2.5f * s * s + 1.0f
	bvals[2] = ((-3.0f * s + 4.0f) * s + 1.0f) * s * 0.5f;		// -1.5f * s * s * s - 2.0f * s * s + 0.5f s
	bvals[3] = ((s - 1.0f) * s * s) * 0.5f;						// 0.5f * s * s * s - 0.5f * s * s
}

/*
====================
xCurve_CatmullRomSpline::BasisFirstDerivative

  first derivative of spline basis functions
====================
*/
template< class type >
X_INLINE void xCurve_CatmullRomSpline<type>::BasisFirstDerivative(const int index, float t, float *bvals) const {
	float s = (float) (t - TimeForIndex(index)) / (TimeForIndex(index+1) - TimeForIndex(index));
	bvals[0] = (-1.5f * s + 2.0f) * s - 0.5f;						// -1.5f * s * s + 2.0f * s - 0.5f
	bvals[1] = (4.5f * s - 5.0f) * s;								// 4.5f * s * s - 5.0f * s
	bvals[2] = (-4.5 * s + 4.0f) * s + 0.5f;						// -4.5 * s * s + 4.0f * s + 0.5f
	bvals[3] = 1.5f * s * s - s;									// 1.5f * s * s - s
}

/*
====================
xCurve_CatmullRomSpline::BasisSecondDerivative

  second derivative of spline basis functions
====================
*/
template< class type >
X_INLINE void xCurve_CatmullRomSpline<type>::BasisSecondDerivative(const int index, float t, float *bvals) const {
	float s = (float) (t - TimeForIndex(index)) / (TimeForIndex(index+1) - TimeForIndex(index));
	bvals[0] = -3.0f * s + 2.0f;
	bvals[1] = 9.0f * s - 5.0f;
	bvals[2] = -9.0f * s + 4.0f;
	bvals[3] = 3.0f * s - 1.0f;
}


/*
===============================================================================

	Cubic Interpolating Spline template.
	The curve goes through all the knots.
	The curve becomes the Catmull-Rom spline if the tension,
	continuity and bias are all set to zero.

===============================================================================
*/

template< class type >
class xCurve_KochanekBartelsSpline : public xCurve_Spline<type> {
public:
						xCurve_KochanekBartelsSpline();

	virtual int			AddValue(float time, const type &value);
	virtual int			AddValue(float time, const type &value, float tension, float continuity, float bias);
	virtual void		RemoveIndex(const int index) { values.RemoveIndex(index); times.RemoveIndex(index); tension.RemoveIndex(index); continuity.RemoveIndex(index); bias.RemoveIndex(index); }
	virtual void		Clear() { values.Clear(); times.Clear(); tension.Clear(); continuity.Clear(); bias.Clear(); currentIndex = -1; }

	virtual type		GetCurrentValue(float time) const;
	virtual type		GetCurrentFirstDerivative(float time) const;
	virtual type		GetCurrentSecondDerivative(float time) const;

protected:
	xArray<float>		tension;
	xArray<float>		continuity;
	xArray<float>		bias;

	void				TangentsForIndex(const int index, type &t0, type &t1) const;

	void				Basis(const int index, float t, float *bvals) const;
	void				BasisFirstDerivative(const int index, float t, float *bvals) const;
	void				BasisSecondDerivative(const int index, float t, float *bvals) const;
};

/*
====================
xCurve_KochanekBartelsSpline::xCurve_KochanekBartelsSpline
====================
*/
template< class type >
X_INLINE xCurve_KochanekBartelsSpline<type>::xCurve_KochanekBartelsSpline() {
}

/*
====================
xCurve_KochanekBartelsSpline::AddValue

  add a timed/value pair to the spline
  returns the index to the inserted pair
====================
*/
template< class type >
X_INLINE int xCurve_KochanekBartelsSpline<type>::AddValue(float time, const type &value) {
	int i;

	i = IndexForTime(time);
	times.Insert(time, i);
	values.Insert(value, i);
	tension.Insert(0.0f, i);
	continuity.Insert(0.0f, i);
	bias.Insert(0.0f, i);
	return i;
}

/*
====================
xCurve_KochanekBartelsSpline::AddValue

  add a timed/value pair to the spline
  returns the index to the inserted pair
====================
*/
template< class type >
X_INLINE int xCurve_KochanekBartelsSpline<type>::AddValue(float time, const type &value, float tension, float continuity, float bias) {
	int i;

	i = IndexForTime(time);
	this->times.Insert(time, i);
	this->values.Insert(value, i);
	this->tension.Insert(tension, i);
	this->continuity.Insert(continuity, i);
	this->bias.Insert(bias, i);
	return i;
}

/*
====================
xCurve_KochanekBartelsSpline::GetCurrentValue

  get the value for the given time
====================
*/
template< class type >
X_INLINE type xCurve_KochanekBartelsSpline<type>::GetCurrentValue(float time) const {
	int i;
	float bvals[4], clampedTime;
	type v, t0, t1;

	if (times.Count() == 1) {
		return values[0];
	}

	clampedTime = ClampedTime(time);
	i = IndexForTime(clampedTime);
	TangentsForIndex(i - 1, t0, t1);
	Basis(i - 1, clampedTime, bvals);
	v = bvals[0] * ValueForIndex(i - 1);
	v += bvals[1] * ValueForIndex(i);
	v += bvals[2] * t0;
	v += bvals[3] * t1;
	return v;
}

/*
====================
xCurve_KochanekBartelsSpline::GetCurrentFirstDerivative

  get the first derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve_KochanekBartelsSpline<type>::GetCurrentFirstDerivative(float time) const {
	int i;
	float bvals[4], d, clampedTime;
	type v, t0, t1;

	if (times.Count() == 1) {
		return (values[0] - values[0]);
	}

	clampedTime = ClampedTime(time);
	i = IndexForTime(clampedTime);
	TangentsForIndex(i - 1, t0, t1);
	BasisFirstDerivative(i - 1, clampedTime, bvals);
	v = bvals[0] * ValueForIndex(i - 1);
	v += bvals[1] * ValueForIndex(i);
	v += bvals[2] * t0;
	v += bvals[3] * t1;
	d = (TimeForIndex(i) - TimeForIndex(i-1));
	return v / d;
}

/*
====================
xCurve_KochanekBartelsSpline::GetCurrentSecondDerivative

  get the second derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve_KochanekBartelsSpline<type>::GetCurrentSecondDerivative(float time) const {
	int i;
	float bvals[4], d, clampedTime;
	type v, t0, t1;

	if (times.Count() == 1) {
		return (values[0] - values[0]);
	}

	clampedTime = ClampedTime(time);
	i = IndexForTime(clampedTime);
	TangentsForIndex(i - 1, t0, t1);
	BasisSecondDerivative(i - 1, clampedTime, bvals);
	v = bvals[0] * ValueForIndex(i - 1);
	v += bvals[1] * ValueForIndex(i);
	v += bvals[2] * t0;
	v += bvals[3] * t1;
	d = (TimeForIndex(i) - TimeForIndex(i-1));
	return v / (d * d);
}

/*
====================
xCurve_KochanekBartelsSpline::TangentsForIndex
====================
*/
template< class type >
X_INLINE void xCurve_KochanekBartelsSpline<type>::TangentsForIndex(const int index, type &t0, type &t1) const {
	float dt, omt, omc, opc, omb, opb, adj, s0, s1;
	type delta;

	delta = ValueForIndex(index + 1) - ValueForIndex(index);
	dt = TimeForIndex(index + 1) - TimeForIndex(index);

	omt = 1.0f - tension[index];
	omc = 1.0f - continuity[index];
	opc = 1.0f + continuity[index];
	omb = 1.0f - bias[index];
	opb = 1.0f + bias[index];
	adj = 2.0f * dt / (TimeForIndex(index + 1) - TimeForIndex(index - 1));
	s0 = 0.5f * adj * omt * opc * opb;
	s1 = 0.5f * adj * omt * omc * omb;

	// outgoing tangent at first point
	t0 = s1 * delta + s0 * (ValueForIndex(index) - ValueForIndex(index - 1));

	omt = 1.0f - tension[index + 1];
	omc = 1.0f - continuity[index + 1];
	opc = 1.0f + continuity[index + 1];
	omb = 1.0f - bias[index + 1];
	opb = 1.0f + bias[index + 1];
	adj = 2.0f * dt / (TimeForIndex(index + 2) - TimeForIndex(index));
	s0 = 0.5f * adj * omt * omc * opb;
	s1 = 0.5f * adj * omt * opc * omb;

	// incoming tangent at second point
	t1 = s1 * (ValueForIndex(index + 2) - ValueForIndex(index + 1)) + s0 * delta;
}

/*
====================
xCurve_KochanekBartelsSpline::Basis

  spline basis functions
====================
*/
template< class type >
X_INLINE void xCurve_KochanekBartelsSpline<type>::Basis(const int index, float t, float *bvals) const {
	float s = (float) (t - TimeForIndex(index)) / (TimeForIndex(index+1) - TimeForIndex(index));
	bvals[0] = ((2.0f * s - 3.0f) * s) * s + 1.0f;				// 2.0f * s * s * s - 3.0f * s * s + 1.0f
	bvals[1] = ((-2.0f * s + 3.0f) * s) * s;					// -2.0f * s * s * s + 3.0f * s * s
	bvals[2] = ((s - 2.0f) * s) * s + s;						// s * s * s - 2.0f * s * s + s
	bvals[3] = ((s - 1.0f) * s) * s;							// s * s * s - s * s
}

/*
====================
xCurve_KochanekBartelsSpline::BasisFirstDerivative

  first derivative of spline basis functions
====================
*/
template< class type >
X_INLINE void xCurve_KochanekBartelsSpline<type>::BasisFirstDerivative(const int index, float t, float *bvals) const {
	float s = (float) (t - TimeForIndex(index)) / (TimeForIndex(index+1) - TimeForIndex(index));
	bvals[0] = (6.0f * s - 6.0f) * s;								// 6.0f * s * s - 6.0f * s
	bvals[1] = (-6.0f * s + 6.0f) * s;							// -6.0f * s * s + 6.0f * s
	bvals[2] = (3.0f * s - 4.0f) * s + 1.0f;						// 3.0f * s * s - 4.0f * s + 1.0f
	bvals[3] = (3.0f * s - 2.0f) * s;								// 3.0f * s * s - 2.0f * s
}

/*
====================
xCurve_KochanekBartelsSpline::BasisSecondDerivative

  second derivative of spline basis functions
====================
*/
template< class type >
X_INLINE void xCurve_KochanekBartelsSpline<type>::BasisSecondDerivative(const int index, float t, float *bvals) const {
	float s = (float) (t - TimeForIndex(index)) / (TimeForIndex(index+1) - TimeForIndex(index));
	bvals[0] = 12.0f * s - 6.0f;
	bvals[1] = -12.0f * s + 6.0f;
	bvals[2] = 6.0f * s - 4.0f;
	bvals[3] = 6.0f * s - 2.0f;
}


/*
===============================================================================

	B-Spline base template. Uses recursive definition and is slow.
	Use xCurve_UniformCubicBSpline or xCurve_NonUniformBSpline instead.

===============================================================================
*/

template< class type >
class xCurve_BSpline : public xCurve_Spline<type> {
public:
						xCurve_BSpline();

	virtual int			GetOrder() const { return order; }
	virtual void		SetOrder(const int i) { assert(i > 0 && i < 10); order = i; }

	virtual type		GetCurrentValue(float time) const;
	virtual type		GetCurrentFirstDerivative(float time) const;
	virtual type		GetCurrentSecondDerivative(float time) const;

protected:
	int					order;

	float				Basis(const int index, const int order, float t) const;
	float				BasisFirstDerivative(const int index, const int order, float t) const;
	float				BasisSecondDerivative(const int index, const int order, float t) const;
};

/*
====================
xCurve_BSpline::xCurve_NaturalCubicSpline
====================
*/
template< class type >
X_INLINE xCurve_BSpline<type>::xCurve_BSpline() {
	order = 4;	// default to cubic
}

/*
====================
xCurve_BSpline::GetCurrentValue

  get the value for the given time
====================
*/
template< class type >
X_INLINE type xCurve_BSpline<type>::GetCurrentValue(float time) const {
	int i, j, k;
	float clampedTime;
	type v;

	if (times.Count() == 1) {
		return values[0];
	}

	clampedTime = ClampedTime(time);
	i = IndexForTime(clampedTime);
	v = values[0] - values[0];
	for (j = 0; j < order; j++) {
		k = i + j - (order >> 1);
		v += Basis(k-2, order, clampedTime) * ValueForIndex(k);
	}
	return v;
}

/*
====================
xCurve_BSpline::GetCurrentFirstDerivative

  get the first derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve_BSpline<type>::GetCurrentFirstDerivative(float time) const {
	int i, j, k;
	float clampedTime;
	type v;

	if (times.Count() == 1) {
		return values[0];
	}

	clampedTime = ClampedTime(time);
	i = IndexForTime(clampedTime);
	v = values[0] - values[0];
	for (j = 0; j < order; j++) {
		k = i + j - (order >> 1);
		v += BasisFirstDerivative(k-2, order, clampedTime) * ValueForIndex(k);
	}
	return v;
}

/*
====================
xCurve_BSpline::GetCurrentSecondDerivative

  get the second derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve_BSpline<type>::GetCurrentSecondDerivative(float time) const {
	int i, j, k;
	float clampedTime;
	type v;

	if (times.Count() == 1) {
		return values[0];
	}

	clampedTime = ClampedTime(time);
	i = IndexForTime(clampedTime);
	v = values[0] - values[0];
	for (j = 0; j < order; j++) {
		k = i + j - (order >> 1);
		v += BasisSecondDerivative(k-2, order, clampedTime) * ValueForIndex(k);
	}
	return v;
}

/*
====================
xCurve_BSpline::Basis

  spline basis function
====================
*/
template< class type >
X_INLINE float xCurve_BSpline<type>::Basis(const int index, const int order, float t) const {
	if (order <= 1) {
		if (TimeForIndex(index) < t && t <= TimeForIndex(index + 1)) {
			return 1.0f;
		} else {
			return 0.0f;
		}
	} else {
		float sum = 0.0f;
		float d1 = TimeForIndex(index+order-1) - TimeForIndex(index);
		if (d1 != 0.0f) {
			sum += (float) (t - TimeForIndex(index)) * Basis(index, order-1, t) / d1;
		}

		float d2 = TimeForIndex(index+order) - TimeForIndex(index+1);
		if (d2 != 0.0f) {
			sum += (float) (TimeForIndex(index+order) - t) * Basis(index+1, order-1, t) / d2;
		}
		return sum;
	}
}

/*
====================
xCurve_BSpline::BasisFirstDerivative

  first derivative of spline basis function
====================
*/
template< class type >
X_INLINE float xCurve_BSpline<type>::BasisFirstDerivative(const int index, const int order, float t) const {
	return (Basis(index, order-1, t) - Basis(index+1, order-1, t)) *
			(float) (order - 1) / (TimeForIndex(index + (order - 1) - 2) - TimeForIndex(index - 2));
}

/*
====================
xCurve_BSpline::BasisSecondDerivative

  second derivative of spline basis function
====================
*/
template< class type >
X_INLINE float xCurve_BSpline<type>::BasisSecondDerivative(const int index, const int order, float t) const {
	return (BasisFirstDerivative(index, order-1, t) - BasisFirstDerivative(index+1, order-1, t)) *
			(float) (order - 1) / (TimeForIndex(index + (order - 1) - 2) - TimeForIndex(index - 2));
}


/*
===============================================================================

	Uniform Non-Rational Cubic B-Spline template.

===============================================================================
*/

template< class type >
class xCurve_UniformCubicBSpline : public xCurve_BSpline<type> {
public:
						xCurve_UniformCubicBSpline();

	virtual type		GetCurrentValue(float time) const;
	virtual type		GetCurrentFirstDerivative(float time) const;
	virtual type		GetCurrentSecondDerivative(float time) const;

protected:
	void				Basis(const int index, float t, float *bvals) const;
	void				BasisFirstDerivative(const int index, float t, float *bvals) const;
	void				BasisSecondDerivative(const int index, float t, float *bvals) const;
};

/*
====================
xCurve_UniformCubicBSpline::xCurve_UniformCubicBSpline
====================
*/
template< class type >
X_INLINE xCurve_UniformCubicBSpline<type>::xCurve_UniformCubicBSpline() {
	order = 4;	// always cubic
}

/*
====================
xCurve_UniformCubicBSpline::GetCurrentValue

  get the value for the given time
====================
*/
template< class type >
X_INLINE type xCurve_UniformCubicBSpline<type>::GetCurrentValue(float time) const {
	int i, j, k;
	float bvals[4], clampedTime;
	type v;

	if (times.Count() == 1) {
		return values[0];
	}

	clampedTime = ClampedTime(time);
	i = IndexForTime(clampedTime);
	Basis(i-1, clampedTime, bvals);
	v = values[0] - values[0];
	for (j = 0; j < 4; j++) {
		k = i + j - 2;
		v += bvals[j] * ValueForIndex(k);
	}
	return v;
}

/*
====================
xCurve_UniformCubicBSpline::GetCurrentFirstDerivative

  get the first derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve_UniformCubicBSpline<type>::GetCurrentFirstDerivative(float time) const {
	int i, j, k;
	float bvals[4], d, clampedTime;
	type v;

	if (times.Count() == 1) {
		return (values[0] - values[0]);
	}

	clampedTime = ClampedTime(time);
	i = IndexForTime(clampedTime);
	BasisFirstDerivative(i-1, clampedTime, bvals);
	v = values[0] - values[0];
	for (j = 0; j < 4; j++) {
		k = i + j - 2;
		v += bvals[j] * ValueForIndex(k);
	}
	d = (TimeForIndex(i) - TimeForIndex(i-1));
	return v / d;
}

/*
====================
xCurve_UniformCubicBSpline::GetCurrentSecondDerivative

  get the second derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve_UniformCubicBSpline<type>::GetCurrentSecondDerivative(float time) const {
	int i, j, k;
	float bvals[4], d, clampedTime;
	type v;

	if (times.Count() == 1) {
		return (values[0] - values[0]);
	}

	clampedTime = ClampedTime(time);
	i = IndexForTime(clampedTime);
	BasisSecondDerivative(i-1, clampedTime, bvals);
	v = values[0] - values[0];
	for (j = 0; j < 4; j++) {
		k = i + j - 2;
		v += bvals[j] * ValueForIndex(k);
	}
	d = (TimeForIndex(i) - TimeForIndex(i-1));
	return v / (d * d);
}

/*
====================
xCurve_UniformCubicBSpline::Basis

  spline basis functions
====================
*/
template< class type >
X_INLINE void xCurve_UniformCubicBSpline<type>::Basis(const int index, float t, float *bvals) const {
	float s = (float) (t - TimeForIndex(index)) / (TimeForIndex(index+1) - TimeForIndex(index));
	bvals[0] = (((-s + 3.0f) * s - 3.0f) * s + 1.0f) * (1.0f / 6.0f);
	bvals[1] = (((3.0f * s - 6.0f) * s) * s + 4.0f) * (1.0f / 6.0f);
	bvals[2] = (((-3.0f * s + 3.0f) * s + 3.0f) * s + 1.0f) * (1.0f / 6.0f);
	bvals[3] = (s * s * s) * (1.0f / 6.0f);
}

/*
====================
xCurve_UniformCubicBSpline::BasisFirstDerivative

  first derivative of spline basis functions
====================
*/
template< class type >
X_INLINE void xCurve_UniformCubicBSpline<type>::BasisFirstDerivative(const int index, float t, float *bvals) const {
	float s = (float) (t - TimeForIndex(index)) / (TimeForIndex(index+1) - TimeForIndex(index));
	bvals[0] = -0.5f * s * s + s - 0.5f;
	bvals[1] = 1.5f * s * s - 2.0f * s;
	bvals[2] = -1.5f * s * s + s + 0.5f;
	bvals[3] = 0.5f * s * s;
}

/*
====================
xCurve_UniformCubicBSpline::BasisSecondDerivative

  second derivative of spline basis functions
====================
*/
template< class type >
X_INLINE void xCurve_UniformCubicBSpline<type>::BasisSecondDerivative(const int index, float t, float *bvals) const {
	float s = (float) (t - TimeForIndex(index)) / (TimeForIndex(index+1) - TimeForIndex(index));
	bvals[0] = -s + 1.0f;
	bvals[1] = 3.0f * s - 2.0f;
	bvals[2] = -3.0f * s + 1.0f;
	bvals[3] = s;
}


/*
===============================================================================

	Non-Uniform Non-Rational B-Spline (NUBS) template.

===============================================================================
*/

template< class type >
class xCurve_NonUniformBSpline : public xCurve_BSpline<type> {
public:
						xCurve_NonUniformBSpline();

	virtual type		GetCurrentValue(float time) const;
	virtual type		GetCurrentFirstDerivative(float time) const;
	virtual type		GetCurrentSecondDerivative(float time) const;

protected:
	void				Basis(const int index, const int order, float t, float *bvals) const;
	void				BasisFirstDerivative(const int index, const int order, float t, float *bvals) const;
	void				BasisSecondDerivative(const int index, const int order, float t, float *bvals) const;
};

/*
====================
xCurve_NonUniformBSpline::xCurve_NonUniformBSpline
====================
*/
template< class type >
X_INLINE xCurve_NonUniformBSpline<type>::xCurve_NonUniformBSpline() {
}

/*
====================
xCurve_NonUniformBSpline::GetCurrentValue

  get the value for the given time
====================
*/
template< class type >
X_INLINE type xCurve_NonUniformBSpline<type>::GetCurrentValue(float time) const {
	int i, j, k;
	float clampedTime;
	type v;
	float *bvals = (float *) _alloca16(order * sizeof(float));

	if (times.Count() == 1) {
		return values[0];
	}

	clampedTime = ClampedTime(time);
	i = IndexForTime(clampedTime);
	Basis(i-1, order, clampedTime, bvals);
	v = values[0] - values[0];
	for (j = 0; j < order; j++) {
		k = i + j - (order >> 1);
		v += bvals[j] * ValueForIndex(k);
	}
	return v;
}

/*
====================
xCurve_NonUniformBSpline::GetCurrentFirstDerivative

  get the first derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve_NonUniformBSpline<type>::GetCurrentFirstDerivative(float time) const {
	int i, j, k;
	float clampedTime;
	type v;
	float *bvals = (float *) _alloca16(order * sizeof(float));

	if (times.Count() == 1) {
		return (values[0] - values[0]);
	}

	clampedTime = ClampedTime(time);
	i = IndexForTime(clampedTime);
	BasisFirstDerivative(i-1, order, clampedTime, bvals);
	v = values[0] - values[0];
	for (j = 0; j < order; j++) {
		k = i + j - (order >> 1);
		v += bvals[j] * ValueForIndex(k);
	}
	return v;
}

/*
====================
xCurve_NonUniformBSpline::GetCurrentSecondDerivative

  get the second derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve_NonUniformBSpline<type>::GetCurrentSecondDerivative(float time) const {
	int i, j, k;
	float clampedTime;
	type v;
	float *bvals = (float *) _alloca16(order * sizeof(float));

	if (times.Count() == 1) {
		return (values[0] - values[0]);
	}

	clampedTime = ClampedTime(time);
	i = IndexForTime(clampedTime);
	BasisSecondDerivative(i-1, order, clampedTime, bvals);
	v = values[0] - values[0];
	for (j = 0; j < order; j++) {
		k = i + j - (order >> 1);
		v += bvals[j] * ValueForIndex(k);
	}
	return v;
}

/*
====================
xCurve_NonUniformBSpline::Basis

  spline basis functions
====================
*/
template< class type >
X_INLINE void xCurve_NonUniformBSpline<type>::Basis(const int index, const int order, float t, float *bvals) const {
    int r, s, i;
    float omega;

    bvals[order-1] = 1.0f;
    for (r = 2; r <= order; r++) {
		i = index - r + 1;
		bvals[order - r] = 0.0f;
		for (s = order - r + 1; s < order; s++) {
			i++;
			omega = (float) (t - TimeForIndex(i)) / (TimeForIndex(i + r - 1) - TimeForIndex(i));
			bvals[s - 1] += (1.0f - omega) * bvals[s];
			bvals[s] *= omega;
		}
    }
}

/*
====================
xCurve_NonUniformBSpline::BasisFirstDerivative

  first derivative of spline basis functions
====================
*/
template< class type >
X_INLINE void xCurve_NonUniformBSpline<type>::BasisFirstDerivative(const int index, const int order, float t, float *bvals) const {
	int i;

	Basis(index, order-1, t, bvals+1);
	bvals[0] = 0.0f;
	for (i = 0; i < order-1; i++) {
		bvals[i] -= bvals[i+1];
		bvals[i] *= (float) (order - 1) / (TimeForIndex(index + i + (order-1) - 2) - TimeForIndex(index + i - 2));
	}
	bvals[i] *= (float) (order - 1) / (TimeForIndex(index + i + (order-1) - 2) - TimeForIndex(index + i - 2));
}

/*
====================
xCurve_NonUniformBSpline::BasisSecondDerivative

  second derivative of spline basis functions
====================
*/
template< class type >
X_INLINE void xCurve_NonUniformBSpline<type>::BasisSecondDerivative(const int index, const int order, float t, float *bvals) const {
	int i;

	BasisFirstDerivative(index, order-1, t, bvals+1);
	bvals[0] = 0.0f;
	for (i = 0; i < order-1; i++) {
		bvals[i] -= bvals[i+1];
		bvals[i] *= (float) (order - 1) / (TimeForIndex(index + i + (order-1) - 2) - TimeForIndex(index + i - 2));
	}
	bvals[i] *= (float) (order - 1) / (TimeForIndex(index + i + (order-1) - 2) - TimeForIndex(index + i - 2));
}


/*
===============================================================================

	Non-Uniform Rational B-Spline (NURBS) template.

===============================================================================
*/

template< class type >
class xCurve_NURBS : public xCurve_NonUniformBSpline<type> {
public:
						xCurve_NURBS();

	virtual int			AddValue(float time, const type &value);
	virtual int			AddValue(float time, const type &value, float weight);
	virtual void		RemoveIndex(const int index) { values.RemoveIndex(index); times.RemoveIndex(index); weights.RemoveIndex(index); }
	virtual void		Clear() { values.Clear(); times.Clear(); weights.Clear(); currentIndex = -1; }

	virtual type		GetCurrentValue(float time) const;
	virtual type		GetCurrentFirstDerivative(float time) const;
	virtual type		GetCurrentSecondDerivative(float time) const;

protected:
	xArray<float>		weights;

	float				WeightForIndex(const int index) const;
};

/*
====================
xCurve_NURBS::xCurve_NURBS
====================
*/
template< class type >
X_INLINE xCurve_NURBS<type>::xCurve_NURBS() {
}

/*
====================
xCurve_NURBS::AddValue

  add a timed/value pair to the spline
  returns the index to the inserted pair
====================
*/
template< class type >
X_INLINE int xCurve_NURBS<type>::AddValue(float time, const type &value) {
	int i;

	i = IndexForTime(time);
	times.Insert(time, i);
	values.Insert(value, i);
	weights.Insert(1.0f, i);
	return i;
}

/*
====================
xCurve_NURBS::AddValue

  add a timed/value pair to the spline
  returns the index to the inserted pair
====================
*/
template< class type >
X_INLINE int xCurve_NURBS<type>::AddValue(float time, const type &value, float weight) {
	int i;

	i = IndexForTime(time);
	times.Insert(time, i);
	values.Insert(value, i);
	weights.Insert(weight, i);
	return i;
}

/*
====================
xCurve_NURBS::GetCurrentValue

  get the value for the given time
====================
*/
template< class type >
X_INLINE type xCurve_NURBS<type>::GetCurrentValue(float time) const {
	int i, j, k;
	float w, b, *bvals, clampedTime;
	type v;

	if (times.Count() == 1) {
		return values[0];
	}

	bvals = (float *) _alloca16(order * sizeof(float));

	clampedTime = ClampedTime(time);
	i = IndexForTime(clampedTime);
	Basis(i-1, order, clampedTime, bvals);
	v = values[0] - values[0];
	w = 0.0f;
	for (j = 0; j < order; j++) {
		k = i + j - (order >> 1);
		b = bvals[j] * WeightForIndex(k);
		w += b;
		v += b * ValueForIndex(k);
	}
	return v / w;
}

/*
====================
xCurve_NURBS::GetCurrentFirstDerivative

  get the first derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve_NURBS<type>::GetCurrentFirstDerivative(float time) const {
	int i, j, k;
	float w, wb, wd1, b, d1, *bvals, *d1vals, clampedTime;
	type v, vb, vd1;

	if (times.Count() == 1) {
		return values[0];
	}

	bvals = (float *) _alloca16(order * sizeof(float));
	d1vals = (float *) _alloca16(order * sizeof(float));

	clampedTime = ClampedTime(time);
	i = IndexForTime(clampedTime);
	Basis(i-1, order, clampedTime, bvals);
	BasisFirstDerivative(i-1, order, clampedTime, d1vals);
	vb = vd1 = values[0] - values[0];
	wb = wd1 = 0.0f;
	for (j = 0; j < order; j++) {
		k = i + j - (order >> 1);
		w = WeightForIndex(k);
		b = bvals[j] * w;
		d1 = d1vals[j] * w;
		wb += b;
		wd1 += d1;
		v = ValueForIndex(k);
		vb += b * v;
		vd1 += d1 * v;
	}
	return (wb * vd1 - vb * wd1) / (wb * wb);
}

/*
====================
xCurve_NURBS::GetCurrentSecondDerivative

  get the second derivative for the given time
====================
*/
template< class type >
X_INLINE type xCurve_NURBS<type>::GetCurrentSecondDerivative(float time) const {
	int i, j, k;
	float w, wb, wd1, wd2, b, d1, d2, *bvals, *d1vals, *d2vals, clampedTime;
	type v, vb, vd1, vd2;

	if (times.Count() == 1) {
		return values[0];
	}

	bvals = (float *) _alloca16(order * sizeof(float));
	d1vals = (float *) _alloca16(order * sizeof(float));
	d2vals = (float *) _alloca16(order * sizeof(float));

	clampedTime = ClampedTime(time);
	i = IndexForTime(clampedTime);
	Basis(i-1, order, clampedTime, bvals);
	BasisFirstDerivative(i-1, order, clampedTime, d1vals);
	BasisSecondDerivative(i-1, order, clampedTime, d2vals);
	vb = vd1 = vd2 = values[0] - values[0];
	wb = wd1 = wd2 = 0.0f;
	for (j = 0; j < order; j++) {
		k = i + j - (order >> 1);
		w = WeightForIndex(k);
		b = bvals[j] * w;
		d1 = d1vals[j] * w;
		d2 = d2vals[j] * w;
		wb += b;
		wd1 += d1;
		wd2 += d2;
		v = ValueForIndex(k);
		vb += b * v;
		vd1 += d1 * v;
		vd2 += d2 * v;
	}
	return ((wb * wb) * (wb * vd2 - vb * wd2) - (wb * vd1 - vb * wd1) * 2.0f * wb * wd1) / (wb * wb * wb * wb);
}

/*
====================
xCurve_NURBS::WeightForIndex

  get the weight for the given index
====================
*/
template< class type >
X_INLINE float xCurve_NURBS<type>::WeightForIndex(const int index) const {
	int n = weights.Count()-1;

	if (index < 0) {
		if (boundaryType == BT_CLOSED) {
			return weights[ weights.Count() + index % weights.Count() ];
		} else {
			return weights[0] + index * (weights[1] - weights[0]);
		}
	} else if (index > n) {
		if (boundaryType == BT_CLOSED) {
			return weights[ index % weights.Count() ];
		} else {
			return weights[n] + (index - n) * (weights[n] - weights[n-1]);
		}
	}
	return weights[index];
}

#endif /* !__X_CURVE_H__ */
