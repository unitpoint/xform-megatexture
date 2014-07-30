#include "xForm.h"

#pragma pack(push,1)
struct TGAHeader
{
  BYTE idLength;
  BYTE colormapType;
  BYTE imageType;
  BYTE colormapSpecification[5];
  WORD xOrigin;
  WORD yOrigin;
  WORD width;
  WORD height;
  BYTE pixelBitCount;
  BYTE imageDescriptor;
};
#pragma pack(pop)

/*
static byte ReadByte(FILE * f)
{
  byte value;
  fread(&value, sizeof(value), 1, f);
  return value;
}
*/

bool xMegaTexture::Make(const xString& filename,
  const xString& dstPrefix, int clusterSize)
{
  ASSERT(xMath::IsPowerOfTwo(clusterSize) && clusterSize >= 64);
  ASSERT(!dstPrefix.IsEmpty());

  FILE * f;
  errno_t err = _tfopen_s(&f, filename.ToChar(), _T("rb"));
  if(err != 0)
    return false;

  TGAHeader tgaHeader;
  if(fread(&tgaHeader, sizeof(tgaHeader), 1, f) != 1)
  {
    fclose(f);
    return false;
  }

  // Only true color, non-mapped images are supported
  if(tgaHeader.colormapType != 0 || tgaHeader.imageType != 2
    || (tgaHeader.pixelBitCount != 24 && tgaHeader.pixelBitCount != 32)
    || tgaHeader.width != tgaHeader.height)
  {
    fclose(f);
    return false;
  }
  // Skip the ID field. The first byte of the header is the length of this field
  if(tgaHeader.idLength)
  {
    fseek(f, tgaHeader.idLength, SEEK_CUR);
  }

  ASSERT(xMath::IsPowerOfTwo(tgaHeader.width));
  ASSERT(xMath::IsPowerOfTwo(tgaHeader.height));
  // ASSERT(tgaHeader.width == tgaHeader.height);

  int width  = tgaHeader.width;
  int height = tgaHeader.height;
  byte * image = new byte[width * height * 3];
  if(!image)
  {
    fclose(f);
    return false;
  }
  byte * line = NULL;
  if(tgaHeader.pixelBitCount == 32)
  {
    line = new byte[width * 4];
    if(!line)
    {
      delete [] image;
      fclose(f);
      return false;
    }
  }

  int x, y;
  for(y = 0; y < height; y++)
  {
    int offs;
    if(!(tgaHeader.imageDescriptor & 0x0010))
    {
      offs = (height-y-1) * width;
    }
    else
    {
      offs = y * width;
    }
    if(line)
    {
      if(fread(line, sizeof(byte)*4*width, 1, f) != 1)
      {
        delete [] line;
        delete [] image;
        fclose(f);
        return false;
      }
      byte * dst = image + offs * 3;
      for(x = 0; x < width; x++, dst += 3)
      {
        dword color[] =
        {
          line[x*4 + 0],
          line[x*4 + 1],
          line[x*4 + 2],
          line[x*4 + 3]
        };
        dst[0] = (color[0] * color[3]) / 0xff;
        dst[1] = (color[1] * color[3]) / 0xff;
        dst[2] = (color[2] * color[3]) / 0xff;
      }
    }
    else
    {
      if(fread(image + offs * 3, sizeof(byte)*3*width, 1, f) != 1)
      {
        delete [] line;
        delete [] image;
        fclose(f);
        return false;
      }
    }
  }
  fclose(f);
  delete [] line;

  MEMSET(&tgaHeader, 0, sizeof(tgaHeader));
  tgaHeader.imageType = 2;
  tgaHeader.pixelBitCount = 24;
  // tgaHeader.imageDescriptor = 0x0010;
  tgaHeader.width = clusterSize;
  tgaHeader.height = clusterSize;

  /*
  Header mageHeader;
  mageHeader.id = ID;
  mageHeader.fullWidth = width;
  mageHeader.fullHeight = height;
  mageHeader.clusterWidth = size;
  mageHeader.clusterHeight = size;
  mageHeader.flags = 0;
  */

  for(int mipmap = 0;; mipmap++)
  {
    for(y = 0; y < height; y += clusterSize)
    {
      for(x = 0; x < width; x += clusterSize)
      {
        xString filename = xString::Format(_T("%s-%d-%d-%dx%d.tga"), dstPrefix, 
          mipmap, clusterSize, x/clusterSize, y/clusterSize);

        err = _tfopen_s(&f, filename.ToChar(), _T("wb"));
        if(err != 0)
        {
          delete [] image;
          return false;
        }

        if(fwrite(&tgaHeader, sizeof(tgaHeader), 1, f) != 1)
        {
          fclose(f);
          delete [] image;
          return false;
        }

        for(int ay = 0; ay < clusterSize; ay++)
        {
          byte * src = image + sizeof(byte)*3 * ((y+clusterSize-1-ay) * width + x);
          if(fwrite(src, sizeof(byte)*3 * clusterSize, 1, f) != 1)
          {
            fclose(f);
            delete [] image;
            return false;
          }
        }
        fclose(f);
      }
    }
    int oldWidth = width;
    int oldHeight = height;
    byte * oldImage = image;
    width /= 2; // Max(clusterSize, width / 2);
    height /= 2; // Max(clusterSize, height / 2);

    if(width >= clusterSize) // || height > clusterSize)
    {
      ASSERT(width == height);
      /*
      image = new byte[width * height * 3];
      if(!image)
      {
        delete [] oldImage;
        return false;
      }
      */

      for(y = 0; y < height; y++)
      {
        for(x = 0; x < width; x++)
        {
          byte * dst = image + sizeof(byte)*3 * (y * height + x);
          byte * src00 = oldImage + sizeof(byte)*3 * (y * 2 * oldHeight + x * 2);
          byte * src01 = src00 + sizeof(byte)*3; // image + sizeof(byte)*3 * (y * oldHeight + x+1);
          byte * src10 = src00 + sizeof(byte)*3 * oldHeight; // image + sizeof(byte)*3 * ((y+1) * oldHeight + x+1);
          byte * src11 = src10 + sizeof(byte)*3; // image + sizeof(byte)*3 * ((y+1) * oldHeight + x);
          for(int i = 0; i < 3; i++)
          {
            dst[i] = (src00[i] + src01[i] + src11[i] + src10[i]) / 4;
          }
        }
      }
      // delete [] oldImage;
    }
    else
    {
      break;
    }
  }

  delete [] image;

  return false;
}

byte * xMegaTexture::FillErrorCluster(byte * buf)
{
  ASSERT(buf && (int)xHeap::Instance()->Size(buf) >= clusterSize * clusterSize * 3);
  byte * dst = buf;
  static byte colors[][3] = 
  {
    {0xC8, 0x00, 0x00},
    {0x30, 0x30, 0x30},
  };
  int colorSize = clusterSize / 8;
  for(int y = 0; y < clusterSize; y++)
  {
    for(int x = 0; x < clusterSize; x++, dst += 3)
    { 
      int i = (x/colorSize ^ y/colorSize) & 1;
      dst[0] = colors[i][0];
      dst[1] = colors[i][1];
      dst[2] = colors[i][2];
    }
  }
  return buf;
}

byte * xMegaTexture::LoadMip(byte * buf, int mip, int px, int py)
{
  if(!buf)
  {
    buf = new byte[clusterSize * clusterSize * 3];
    ASSERT(buf);
  }
  xString mipFilename = xString::Format(_T("%s-%d-%d-%dx%d.tga"), filename, mip, clusterSize, px, py);

  FILE * f;
  errno_t err = _tfopen_s(&f, mipFilename.ToChar(), _T("rb"));
  if(err != 0)
  {
    return FillErrorCluster(buf);
  }

  TGAHeader tgaHeader;

  if(fread(&tgaHeader, sizeof(tgaHeader), 1, f) != 1)
  {
    fclose(f);
    return FillErrorCluster(buf);
  }
  // Only true color, non-mapped images are supported
  if(tgaHeader.colormapType != 0 || tgaHeader.imageType != 2 || tgaHeader.pixelBitCount != 24
      || tgaHeader.idLength)
  {
    fclose(f);
    return FillErrorCluster(buf);
  }

  ASSERT(tgaHeader.width == clusterSize);
  ASSERT(tgaHeader.height == clusterSize);

  int srcRowSize = sizeof(byte) * 3 * tgaHeader.width;
  int srcSize = srcRowSize * tgaHeader.height;
  byte * src = new byte[srcSize];
  ASSERT(src);

  if(fread(src, srcSize, 1, f) != 1)
  {
    fclose(f);
    delete [] src;
    return FillErrorCluster(buf);
  }
  fclose(f);

  byte * srcRow = src;
  for(int y = 0; y < tgaHeader.height; y++)
  {
    int offs;
    if(!(tgaHeader.imageDescriptor & 0x0010))
    {
      offs = (tgaHeader.height-y-1) * tgaHeader.width;
    }
    else
    {
      offs = y * tgaHeader.width;
    }
    MEMCPY(buf + offs * 3, srcRow, srcRowSize);
    srcRow += srcRowSize;
  }

  delete [] src;

  return buf;
}

void xMegaTexture::UpdateLayer(int layerNum, int x, int y, int width, int height)
{
  ASSERT(layerNum >= 0 && layerNum < layersNumber);
  MipLayer * layer = layers + layerNum;
  if(layer->x == x && layer->y == y && layer->width == width && layer->height == height)
  {
    return;
  }

  int oldSize = layer->width * layer->height;
  Cluster ** oldClusters = layer->clusters;

  int newSize = width * height;
  layer->clusters = new Cluster*[newSize];
  ASSERT(layer->clusters);

  layer->x = x;
  layer->y = y;
  layer->width = width;
  layer->height = height;

  int offs, i, j, k;
  for(offs = j = 0; j < height; j++)
  {
    for(i = 0; i < width; i++, offs++)
    {
      k = FindClusterNum(oldClusters, oldSize, x+i, y+j);
      if(k >= 0)
      {
        layer->clusters[offs] = oldClusters[k];
        oldClusters[k] = NULL;
      }
    }
  }

  int fromIndex = oldSize - 1;
  for(offs = j = 0; j < height; j++)
  {
    for(i = 0; i < width; i++, offs++)
    {
      if(!layer->clusters[offs])
      {
        for(; fromIndex >= 0; fromIndex--)
        {
          if(oldClusters[fromIndex])
            break;
        }
        if(fromIndex >= 0)
        {
          layer->clusters[offs] = oldClusters[fromIndex];
          oldClusters[fromIndex--] = NULL;
        }
        else
        {
          layer->clusters[offs] = new Cluster();
          // layer->clusters[offs]->image = new byte[clusterSize * clusterSize * 3];
          // ASSERT(layer->clusters[offs]->image);
        }
        layer->clusters[offs]->x = x+i;
        layer->clusters[offs]->y = y+j;
        layer->clusters[offs]->image = LoadMip(layer->clusters[offs]->image, layerNum, x+i, y+j);
      }
    }
  }

  for(k = 0; k < oldSize; k++)
  {
    delete oldClusters[k];
  }
  delete [] oldClusters;
}

void xMegaTexture::GetTexture(int layerNum, int x, int y, int width, int height, 
  byte * dst, int pitch, int dstWidth, int dstHeight, int pixelBits)
{
  ASSERT(pixelBits == 24 || pixelBits == 32);
  ASSERT(dstWidth == width * clusterSize);
  ASSERT(dstHeight == height * clusterSize);
  ASSERT(layerNum >= 0 && layerNum < layersNumber);
  
  MipLayer * layer = layers + layerNum;
  ASSERT(layer->x <= x && layer->y <= y && layer->width <= x + width && layer->height <= y + height);

  int rowSize = sizeof(byte) * 3 * clusterSize;
  int srcRowSize = sizeof(byte) * (pixelBits / 8) * clusterSize;
  for(int j = 0; j < height; j++)
  {
    for(int i = 0; i < width; i++)
    {
      // int offsX = x - layer->x + i;
      // int offsY = y - layer->y + j;
      Cluster * cluster = layer->FindCluster(x + i, y + j);
      ASSERT(cluster);

      byte * clusterDest = dst + (j * clusterSize * pitch + i * srcRowSize);
      byte * src = cluster->image;
      
      if(pixelBits == 24)
      {
        for(int row = 0; row < clusterSize; row++)
        {
          MEMCPY(clusterDest, src, rowSize);
          src += rowSize;
          clusterDest += pitch;
        }
      }
      else // do need convert src to 32 bits per pixel ?
      {
        for(int row = 0; row < clusterSize; row++)
        {
          byte * rowDest = clusterDest;
          for(int col = 0; col < clusterSize; col++)
          {
            *rowDest++ = *src++;
            *rowDest++ = *src++;
            *rowDest++ = *src++;
            *rowDest++ = 0xff;
          }
          clusterDest += pitch;
        }
      }
    }
  }
}

xMegaTexture::xMegaTexture()
{
  layers = NULL;
  layersNumber = 0;
  clusterSize = 0;
  layerSize = 0;
}

xMegaTexture::~xMegaTexture()
{
  delete [] layers;
}

void xMegaTexture::Init(const xString& p_filename, int p_layersNumber, int p_clusterSize, int p_layerSize)
{
  filename = p_filename;
  clusterSize = p_clusterSize;

  delete [] layers;

  if(!p_layersNumber)
  {
    layers = NULL;
    layersNumber = 0;
    layerSize = 0;
    return;
  }

  layerSize = p_layerSize;
  layersNumber = p_layersNumber;
  layers = new MipLayer[layersNumber];
  ASSERT(layers);
}
