[一看就懂的OpenGL ES教程——图形渲染管线的那些事](https://juejin.cn/post/7119135465302654984)


# 图元类型

### 1.1  

- GL_POINTS 每个顶点在屏幕上都是单独的点
- GL_LINES 每一对顶点定义一个线段
- GL_LINE_STRIP 一个从第一个顶点依次经过每一个后续顶点而绘制的线条
- GL_TRIANGLES 每三个顶点定义一个新的三角形
- GL_TRIANGLE_STRIP 共用一条带（strip）上的顶点的一组三角形
- GL_TRIANGLE_FAN 以一个圆为中心呈扇形排列，共用相邻点的一组三角形


`glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);`
这是OpenGL的绘制指令，在它前面的一系列指令只是配置，而glDrawArrays就像一个流水线开关，一踩下去，
整个流水线才真正动起来，传入OpenGL的数据才像一条河流一样在一个个阶段流动起来，直到形成的图像数据被绘制到对应的帧缓冲~



`glClearColor(1.0f, 1.0f, 1.0f, 1.0f);`
将颜色缓冲清空为什么颜色，参数为对应的RGBA值

glClear(GL_COLOR_BUFFER_BIT);
真正将颜色缓冲设置为glClearColor指定的值。可以说glClearColor函数是一个状态设置函数，而glClear函数则是一个状态使用的函数
glClear方法的参数为指定要设置的缓冲区，可以传入的数值为：GL_COLOR_BUFFER_BIT、GL_DEPTH_BUFFER_BIT 和 GL_STENCIL_BUFFER_BIT，分别表示颜色缓冲、深度缓冲、模板缓冲。

前面说过光栅化后形成的片段包含绘制一个像素的所有信息，包含颜色、深度、模板等，所有这些信息会缓存在3个缓冲区中，
分别就是颜色缓冲（color buffer）、深度缓冲(depth buffer)、模板缓冲（stencil buffer）。


## Matrix.frustumM
需要填充的参数有
- float left, //near面的left   相对观察点近面的左边距
- float right, //near面的right 相对观察点近面的右边距
- float bottom, //near面的bottom
- float top, //near面的top
- float near, //near面距离
- float far //far面距离

设置这些参数能起到的作用：先是left，right和bottom,top，这4个参数会影响图像左右和上下缩放比，
所以往往会设置的值分别-(float) width / height和(float) width / height，
top和bottom和top会影响上下缩放比，如果left和right已经设置好缩放，则bottom只需要设置为-1，
top设置为1，这样就能保持图像不变形。

也可以将left，right 与bottom，top交换比例， 即bottom和top设置为 -height/width 和 height/width, left和right设置为-1和1。

near和far参数稍抽象一点，就是一个立方体的前面和后面，near和far需要结合拍摄相机即观察者眼睛的位置来设置，
例如setLookAtM中设置cx = 0, cy = 0, cz = 10，near设置的范围需要是小于10才可以看得到绘制的图像，
如果大于10，图像就会处于了观察这眼睛的后面，这样绘制的图像就会消失在镜头前，
far参数，far参数影响的是立体图形的背面，far一定比near大，一般会设置得比较大，如果设置的比较小，
一旦3D图形尺寸很大，这时候由于far太小，这个投影矩阵没法容纳图形全部的背面，这样3D图形的背面会有部分隐藏掉的

## Matrix.setLookAtM
需要填充的参数
- float cx, //摄像机位置x
- float cy, //摄像机位置y
- float cz, //摄像机位置z
- float tx, //摄像机目标点x
- float ty, //摄像机目标点y
- float tz, //摄像机目标点z
- float upx, //摄像机UP向量X分量  
- float upy, //摄像机UP向量Y分量
- float upz //摄像机UP向量Z分量

这个方法看起来很抽象，设几组参数对比一下效果，摄像机目标点，
即绘制的3D图像，tx，ty, tz,为图像的中心位置设置到原点即 tx = 0,ty = 0, tz = 0; 
摄像机的位置，即观察者眼睛的位置 我们设置在目标点的正前方（位置z轴正方向），cx = 0, cy = 0, cz = 10; 
接着是摄像机顶部的方向了，如下图，很显然相机旋转，up的方向就会改变，这样就会会影响到绘制图像的角度。

眼睛正常 （upx =0,upy=1,upz= 0）
眼睛旋转180度（upx =0,upy=-1,upz= 0） 
其他类似


# 上传符号表
java -jar D:\file\videostream\buglyqq-upload-symbol\buglyqq-upload-symbol.jar -appid b5500b224d -appkey ce0a9054-2c12-4b0a-9399-b23fa9740d54 -bundleid com.shixin.ndk_practice -version 1.0 -platform Android -inputSymbol D:\file\asproject\ndk_practice\app\build\intermediates\cmake\debug\obj\armeabi-v7a\



## glDrawElements

//从indices中按顺序取出索引对应6个顶点依次进行绘制，图元类型为GL_TRIANGLES
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);

第一个参数mode:
和glDrawArrays方法一样，依然表示要绘制的具体图元类型（关于图元绘制相关，可以看下一看就懂的OpenGL ES教程——这或许是你遇过最难画的三角形（四））
第二个参数count:
表示要绘制多少个顶点，比如绘制一个三角形那就是3个顶点。
第三个参数type:
表示的是顶点索引的类型，必须是GL_UNSIGNED_BYTE,GL_UNSIGNED_SHORT, or GL_UNSIGNED_INT其中一个种。
第四个参数indices:
就是索引数组的指针。
绘制代码什么样的呢？


## 光照

```
attribute vec4 aPosition;
attribute vec2 aCoord;
attribute vec3 aNormal;
uniform mat4 uMatrix;
uniform vec4 uBaseColor;
uniform vec3 uLightColor;
uniform float uAmbientStrength;
uniform float uDiffuseStrength;
uniform float uSpecularStrength;
uniform vec3 uLightPosition;
varying vec4 vColor;

//在片元着色器中计算光照会获得更好更真实的光照效果，但是会比较耗性能

//环境光的计算
vec4 ambientColor(){
vec3 ambient = uAmbientStrength * uLightColor;
return vec4(ambient,1.0);
}

//漫反射的计算
vec4 diffuseColor(){
//模型变换后的位置
vec3 fragPos=(uMatrix*aPosition).xyz;
//光照方向
vec3 direction=normalize(uLightPosition-fragPos);
//模型变换后的法线向量
vec3 normal=normalize(mat3(uMatrix)*aNormal);
//max(cos(入射角)，0)
float diff = max(dot(normal,direction), 0.0);
//材质的漫反射系数*max(cos(入射角)，0)*光照颜色
vec3 diffuse=uDiffuseStrength * diff * uLightColor;
return vec4(diffuse,1.0);
}

//镜面光计算，镜面光计算有两种方式，一种是冯氏模型，一种是Blinn改进的冯氏模型
//这里使用的是改进的冯氏模型，基于Half-Vector的计算方式
vec4 specularColor(){
//模型变换后的位置
vec3 fragPos=(uMatrix*aPosition).xyz;
//光照方向
vec3 lightDirection=normalize(uLightPosition-fragPos);
//模型变换后的法线向量
vec3 normal=normalize(mat3(uMatrix)*aNormal);
//观察方向，这里将观察点固定在（0，0，uLightPosition.z）处
vec3 viewDirection=normalize(vec3(0,0,uLightPosition.z)-fragPos);
//观察向量与光照向量的半向量
vec3 hafVector=normalize(lightDirection+viewDirection);
//max(0,cos(半向量与法向量的夹角)^粗糙度
float diff=pow(max(dot(normal,hafVector),0.0),4.0);
//材质的镜面反射系数*max(0,cos(反射向量与观察向量夹角)^粗糙度*光照颜色
//材质的镜面反射系数*max(0,cos(半向量与法向量的夹角)^粗糙度*光照颜色
vec3 specular=uSpecularStrength*diff*uLightColor;
return vec4(specular,1.0);
}

void main(){
gl_Position=uMatrix*aPosition;
vColor=(ambientColor() + diffuseColor() + specularColor())* uBaseColor;
}

 ```