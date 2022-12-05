//暖色滤镜
// 设置精度，中等精度
precision mediump float;
// varying 可用于相互传值
varying vec2 ft_Position;
// 2D 纹理 ，uniform 用于 application 向 gl 传值
uniform sampler2D sTexture;
void main() {
    vec4 nColor = texture2D(sTexture, ft_Position);//进行纹理采样,拿到当前颜色
    gl_FragColor = nColor + vec4(0.2, 0.2, 0.0, 0.0); //暖就是多加点红跟绿
}