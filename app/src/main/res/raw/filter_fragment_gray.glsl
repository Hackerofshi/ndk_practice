// 设置精度，中等精度
precision mediump float;
// varying 可用于相互传值
varying vec2 ft_Position;
// 2D 纹理 ，uniform 用于 application 向 gl 传值
uniform sampler2D sTexture;
void main() {
    vec4 nColor = texture2D(sTexture, ft_Position);//进行纹理采样,拿到当前颜色
    float c = nColor.r * 0.30 + nColor.g * 0.59 + nColor.b * 0.11;
    gl_FragColor=vec4(c, c, c, nColor.a);
}