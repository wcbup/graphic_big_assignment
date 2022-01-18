#version 330 

in vec2 TexCoord0;
in vec3 Normal0;

out vec4 FragColor;

struct DirectionalLight
{
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
    vec3 Direction;
};

struct Material
{
    vec3 AmbientColor;
    vec3 DiffuseColor;
};

uniform DirectionalLight gDirectionalLight;
uniform Material gMaterial;
uniform sampler2D gSampler;
uniform int hasTexture;

float getGrayScale(vec4 color)
{
    return color.x * 0.299f + color.y * 0.587f + color.z * 0.114f;
}

vec4 getColor(vec2 position)
{
    return texture2D(gSampler, position);
}

float getSobelGray(mat3 operator)
{
    float offset = 0.0001f;
    float grayScale = 0;
    for(int i=0; i<3; i++)
    {
        for(int j=0; j<3; j++)
        {
            grayScale += 
                operator[i][j] * getGrayScale(getColor(vec2(TexCoord0.x + (i-1)*offset, TexCoord0.y + (j-1)*offset)));
        }
    }
    return grayScale;
}

vec4 getSobelColor()
{
    float totalGray = 0.0f;
    mat3 Gx = mat3(-1.0f, 0.0f, 1.0f,
            -2.0f, 0.0f, 2.0f,
            -1.0f, 0.0f, 1.0f);
    mat3 Gy = mat3(1.0f, 2.0f, 1.0f,
            0.0f, 0.0f, 0.0f,
            -1.0f, -2.0f, -1.0f);
    float gxScale = getSobelGray(Gx);
    float gyScale = getSobelGray(Gy);
    totalGray = sqrt(gxScale*gxScale + gyScale*gyScale);
    return vec4(totalGray, totalGray, totalGray, 1.0f);
}

void main()
{
    vec4 AmbientColor = vec4(gDirectionalLight.Color, 1.0f) *
                        gDirectionalLight.AmbientIntensity *
                        vec4(gMaterial.AmbientColor, 1.0f);
    float DiffuseFactor = dot(normalize(Normal0), -gDirectionalLight.Direction);

    vec4 DiffuseColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);

    if(DiffuseFactor > 0)
    {
        DiffuseColor = vec4(gDirectionalLight.Color, 1.0f) *
                       gDirectionalLight.DiffuseIntensity *
                       vec4(gMaterial.DiffuseColor, 1.0f) *
                       DiffuseFactor;
    }

    if(hasTexture == 0)
    {
        FragColor = vec4(0.8f ,0.8f, 0.8f, 1) * (AmbientColor + DiffuseColor);
    }
    else
    {
        FragColor = texture2D(gSampler, TexCoord0.xy) *
                    (AmbientColor + DiffuseColor);
    }

    // if(FragColor == vec4(0,0,0,0))
    // {
    //     FragColor = vec4(1,1,1,1) * (AmbientColor + DiffuseColor);
    // }

    // FragColor = getSobelColor();
}
