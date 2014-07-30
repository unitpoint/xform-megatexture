#include "app.h"

// =================================================================
// =================================================================
// =================================================================

xCarBotEntity::xCarBotEntity(xWorld * world, xEntity * dest): xCarEntity(world)
{ 
  this->dest = NULL;
  SetEntityPtr(this->dest, dest);
  blockTime = -1;
}

void xCarBotEntity::DeadNotify(xEntity * deadEntity)
{
  if(deadEntity == dest)
    dest = NULL;
}

void xCarBotEntity::Think()
{
  if(!dest){
    UpdateDriverControls(0.0f, 0, State().controls.steering, 0, true);
    return;
  }

  xAngles angles;
  xVec3 origin, destPos = dest->Origin();
  GetPosition(origin, angles);

  destPos.z = 0;
  origin.z = 0;
  xVec3 destVec = destPos - origin;
  xVec3 localDir = WorldDirToLocal(destVec.Norm());
  float destYaw = xAngles::Norm180(xAngles(localDir, vec3_up).yaw);
  float steering = Clamp(-destYaw / Params().steering.maxAngle, -1.0f, 1.0f);

  float destLen = destVec.LengthFast();
  float speed = State().speed;
  float timeToDest = Max(0.1f, destLen) / Max(0.1f, speed);
  if(destLen < 3){
    /*if(!speed){
      UpdateDriverControls(0.0f, 0, steering, 0, false);
    }else*/
      UpdateDriverControls(0.0f, 0, State().controls.steering, 0, true);
  }else{
    float time = World()->Time();
    float gaz = Min(1.0f, destLen / 50.0f);
    if(reverseTime > 0){
      float maxReverseTime = 5;
      if(time - reverseTime < maxReverseTime){
        gaz = -1;
        steering *= (time - reverseTime) / maxReverseTime - 1;
      }else{
        reverseTime = -1;
        blockTime = -1;
      }
    }else if(blockTime < 0){
      if(speed < xPhysConvert::SpeedKMH2MS(1))
        blockTime = time;
    }else{
      if(speed > xPhysConvert::SpeedKMH2MS(1))
        blockTime = -1;
      else if(time - blockTime > 5){
        reverseTime = time;
      }
    }
    UpdateDriverControls(gaz, 0, steering, 0, false);
  }

  xEntCarVehicle::Think();
}