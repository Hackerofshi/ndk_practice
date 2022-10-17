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