#!/bin/bash

echo Converting ISPC tutorial $1 to CPP tutorial $2
cp $1 $2
sed -i .backup  's/.isph\"/.h\"/g' $2
sed -i .backup  's/RTC_INTERSECT_UNIFORM | RTC_INTERSECT_VARYING/RTC_INTERSECT1/g' $2
sed -i .backup  's/uniform //g' $2
sed -i .backup  's/ uniform//g' $2
sed -i .backup  's/varying //g' $2
sed -i .backup  's/ varying//g' $2
sed -i .backup  's/extern/extern \"C\"/g' $2
sed -i .backup  's/export/extern \"C\"/g' $2
sed -i .backup  's/launch\[numTilesX\*numTilesY\] renderTile(/launch_renderTile(numTilesX\*numTilesY,/g' $2
sed -i .backup  's/launch\[numPhi+1\] animateSphere(/launch_animateSphere(numPhi+1,/g' $2
sed -i .backup  's/\*pi\*/\*float(pi)\*/g' $2
sed -i .backup  's/\*pi\//\*float(pi)\//g' $2
#sed -i .backup  's/RTC_MATRIX_COLUMN_MAJOR/RTC_MATRIX_COLUMN_MAJOR_ALIGNED16/g' $2
sed -i .backup  's/sync;//g' $2
sed -i .backup  's/make_//g' $2
sed -i .backup  's/\#if 0 \/\/ FIXME: pointer gather/\#if 1 \/\/ FIXME: pointer gather/g' $2
sed -i .backup  's/foreach (y = y0 ... y1, x = x0 ... x1)/for (int y = y0; y<y1; y++) for (int x = x0; x<x1; x++)/g' $2
sed -i .backup  's/foreach (phi = 0 ... numPhi+1, theta = 0 ... numTheta)/for (int phi = 0; phi <numPhi+1; phi++) for (int theta = 0; theta<numTheta; theta++)/g' $2
sed -i .backup  's/foreach (theta = 0 ... numTheta)/for (int theta = 0; theta<numTheta; theta++)/g' $2
sed -i .backup  's/task void renderTile(int\* pixels,/void renderTile(int taskIndex, int\* pixels,/g' $2
sed -i .backup  's/task void animateSphere (Vertex\* vertices,/void animateSphere (int taskIndex, Vertex\* vertices,/g' $2
sed -i .backup  's/Vec3f renderPixelStandard(float x, float y, const Vec3f\& vx, const Vec3f\& vy, const Vec3f\& vz, const Vec3f\& p)/Vec3fa renderPixelStandard(float x, float y, const Vec3fa\& vx, const Vec3fa\& vy, const Vec3fa\& vz, const Vec3fa\& p)/g' $2
sed -i .backup  's/RTCIntersectFuncVarying/RTCIntersectFunc/g' $2
sed -i .backup  's/RTCOccludedFuncVarying/RTCOccludedFunc/g' $2
sed -i .backup  's/\#if 1 \/\/ enables parallel execution/\#if 0/g' $2
sed -i .backup  's/RTCFilterFuncVarying/RTCFilterFunc/g' $2
sed -i .backup  's/Vec3f org/Vec3fa org/g' $2
sed -i .backup  's/Vec3f dir/Vec3fa dir/g' $2
sed -i .backup  's/Vec3f Ng/Vec3fa Ng/g' $2

