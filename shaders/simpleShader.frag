void main()
{
    mediump float z = gl_FragCoord.z * 0.1;
    //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    gl_FragColor = vec4(z, z, z, 1);
}
