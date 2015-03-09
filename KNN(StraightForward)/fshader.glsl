#ifdef GL_ES
// Set default precision to high
precision mediump int;
precision mediump float;
#endif

//RT2DShader

varying vec2 v_texcoord;

uniform sampler2D WeightsLayer;
uniform sampler2D ActivationLayer;
uniform int size;
uniform int numLayers;

const float learningRate = 0.656;

const float pi = 3.14159265359;

uniform int currLayer;
uniform int shaderMode;

vec4 pack( float v ) {
  vec4 enc = vec4(1.0, 255.0, 65025.0, 160581375.0) * v;
  enc = fract(enc);
  enc -= enc.yzww * vec4(1.0/255.0,1.0/255.0,1.0/255.0,0.0);
  return enc;
}
float unpack( vec4 rgba ) {
  return dot( rgba, vec4(1.0, 1/255.0, 1/65025.0, 1/160581375.0) );
}
//synthesizing TexelFetch
vec4 texelFetch(sampler2D tex,const highp vec2 coord, const highp vec2 size){
    highp vec2 fCoord = vec2((2.0*float(coord.x) /* - 1.0 */ )/(2.0*float(size.x)),(2.0*float(coord.y) /* - 1.0 */)/(2.0*float(size.y)));
    return texture2D(tex,fCoord);
}
//sigmoide activation function
float sigmoid(float x){
    return (1.0 / (1.0+exp(-x)));
}
//Map from 0..1 to -1..1 and unmap back
highp float map(highp float val){
    return ((2.0*val)-1.0);
}
highp float unmap(highp float val){
    return (0.5*(val+1.0));
}
float clip(float val){
    if (val >= 1.0) return 0.9999;
    if (val <= 0.0) return 0.0000;
    return val;
}


void main()
{
    //Pixel Error Code (just for Debugging puposes)
    vec4 imagePixel = vec4(1.0,1.0,1.0,1.0);

    //Propagate Backwards (=2), Adjust WeightsLayer (=3)
    if(shaderMode == 2 || shaderMode == 3){

        if(shaderMode == 2){
            //Performence lag here !
            float errorOut = map(unpack(texture2D(WeightsLayer,vec2(v_texcoord.x,float(size)/(2.0*(float(size)+1.0))))));
            float outPrevLayer = unpack(texture2D(ActivationLayer,vec2(v_texcoord.x,-1.0/(2.0*(float(size)+1.0)))));
            //float inputActivation = unpack(texture2D(ActivationLayer,vec2(v_texcoord.x,-1.0/(2.0*(float(size)+1.0)))));

            highp float sumPixelX = 0.0;
            //Tex offset by 1
            for(int y = -1; y < size-1; y++){
                float weight = map(unpack(texelFetch(WeightsLayer,vec2(y,v_texcoord.x*(size+1)),vec2(size,size+1))));
                sumPixelX = sumPixelX + errorOut * weight;
            }
            imagePixel = pack(unmap(sumPixelX*outPrevLayer*(1.0-outPrevLayer)));

        }else{

            float weight = map(unpack(texture2D(WeightsLayer,v_texcoord)));
            float inputN = unpack(texture2D(ActivationLayer,vec2(v_texcoord.x,-1.0/(2.0*(float(size)+1.0)))));
            float errorTerm = map(unpack(texture2D(WeightsLayer,vec2(v_texcoord.y*(float(size)/float(size+1)),(2.0*float(size+1) + 1.0)/(2.0*(float(size)+1.0))))));

            imagePixel = pack(clip(unmap(weight + errorTerm * inputN * learningRate)));
            //imagePixel = pack(unmap(errorTerm));
            //imagePixel = texelFetch(WeightsLayer,vec2(v_texcoord.x * size, v_texcoord.y * (size+1)),vec2(size,size+1));
        }

    }

    //Propagate Forward (=0)/Forward Sum(=1) through the Layers
    if(shaderMode == 0 || shaderMode == 1){

        //Forward Propagation
        if(shaderMode == 0){

            //offset by 1
            float activation = unpack(texture2D(ActivationLayer,vec2(v_texcoord.x,-1.0/(2.0*(float(size)+1.0)))));
            float weight     = map(unpack(texture2D(WeightsLayer,v_texcoord)));

            imagePixel = pack(clip(unmap(weight * activation)));

        }else{

               //Performence lag here !
                highp float sumPixelX = 0.0;
                for(int y = 0; y < size; y++){
                    sumPixelX += map(unpack(texelFetch(ActivationLayer,vec2(float(y),v_texcoord.x*float(size-1)),vec2(float(size),float(size+1)))));
                }
                imagePixel = pack(clip(sigmoid(sumPixelX)));
                //imagePixel = texture2D(ActivationLayer,vec2(((size-1)*2.0)/(size*2.0),v_texcoord.x));
                //imagePixel = pack(1.0-v_texcoord.x);

        }
    }

    //Display the Weights and the internal States
    if(shaderMode == -1){
        if(v_texcoord.x <= 0.5){
            float weights = map(unpack(texture2D(WeightsLayer,vec2(v_texcoord.x*2.0,v_texcoord.y))));
            if(weights >= 0.0) imagePixel = vec4(weights,0.0,0.0,1.0);
            if(weights <= 0.0) imagePixel = vec4(0.0,0.0,-weights,1.0);
            //imagePixel = texture2D(WeightsLayer,vec2(v_texcoord.x*2.0,v_texcoord.y));
        }else{
            float activations = map(unpack(texture2D(ActivationLayer,vec2((v_texcoord.x-0.5)*2.0,v_texcoord.y))));
            if(activations >= 0.0) imagePixel = vec4(activations,0.0,0.0,1.0);
            if(activations <= 0.0) imagePixel = vec4(0.0,0.0,-activations,1.0);
            //imagePixel = texture2D(ActivationLayer,vec2((v_texcoord.x-0.5)*2.0,v_texcoord.y));
        }
    }

    gl_FragColor = imagePixel;

}

