// 着色器纹理扩展类型
#extension GL_OES_EGL_image_external : require
// 设置精度，中等精度
precision mediump float;
// varying 可用于相互传值
varying vec2 ft_Position;
// 2D 纹理 ，uniform 用于 application 向 gl 传值 （扩展纹理）
uniform samplerExternalOES sTexture;
void main() {
    // 取相应坐标点的范围转成 texture2D
    gl_FragColor=texture2D(sTexture, ft_Position);
}
