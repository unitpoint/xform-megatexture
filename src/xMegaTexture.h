#ifndef __X_MEGA_TEXTURE__
#define __X_MEGA_TEXTURE__

#pragma once

#include "common/xString.h"

class xMegaTexture
{
public:
  
  /* enum
  {
    ID = 0xabcdef01
  }; */

  struct Cluster
  {
    int x, y;
    byte * image;

    Cluster(){ x = y = 0; image = NULL; }
    ~Cluster(){ delete [] image; }
  };

  static int FindClusterNum(Cluster ** clusters, int size, int x, int y)
  {
    for(int i = 0; i < size; i++)
    {
      if(clusters[i] && clusters[i]->x == x && clusters[i]->y == y)
        return i;
    }
    return -1;
  }

  struct MipLayer
  {
    int width, height;
    // int mip;
    int x, y;

    Cluster ** clusters;

    MipLayer()
    {
      width = height = 0;
      x = y = 0;
      // mip = 0;
      clusters = NULL;
    }
    ~MipLayer()
    {
      int size = width * height;
      for(int i = 0; i < size; i++)
      {
        delete clusters[i];
      }
      delete [] clusters;
    }

    Cluster * FindCluster(int x, int y)
    {
      int i = FindClusterNum(clusters, width * height, x, y);
      return i >= 0 ? clusters[i] : NULL;
    }
  };

protected:

  /* enum
  {
    CLUSTER_ERROR_POS = -1000000
  };

  struct Header
  {
    int id;
    int fullWidth;
    int fullHeight;
    int clusterSize;
    int flags;
  }; */

  MipLayer * layers;
  int layersNumber;

  xString filename;
  int clusterSize;
  int layerSize;
  
  byte * FillErrorCluster(byte * buf);
  byte * LoadMip(byte * buf, int mip, int x, int y);

  // byte * Cluster(MipLayer * layer, int i, int j);

public:

  xMegaTexture();
  ~xMegaTexture();

  void Init(const xString& p_filename, int p_layersNumber, int p_clusterSize, int p_layerSize);

  // void UpdateLayers(int x, int y, int width, int height);
  void UpdateLayer(int layerNum, int x, int y, int width, int height);
  void GetTexture(int layerNum, int x, int y, int width, int height, 
    byte * dst, int pitch, int dstWidth, int dstHeight, int pixelBits);

  int ClusterSize() const { return clusterSize; }

  static bool Make(const xString& filename,
    const xString& dstPrefix, int clusterSize = 128);
};

#endif // __X_MEGA_TEXTURE__
