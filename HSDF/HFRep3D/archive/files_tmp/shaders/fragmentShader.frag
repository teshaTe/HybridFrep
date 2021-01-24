#version 330

in  vec4 vCol;

in  vec3 texCoord3D_0; // sdf field coordinates
in  vec3 texCoord3D_1; // background coordinates

in  vec3 FragPos;

out vec4 colour;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS  = 3;

//ray marching params
struct RayMarching
{
    int   numberOfSteps;
    float stopThres;
    float gradStep;
    float clipFar;
    float maxTraceDist;
    float minHitDist;
};

//settting up three types of light
struct light
{
    vec3  colour;
    float ambientIntensity;
    float diffuseIntensity;
};

struct DirectionalLight
{
    light base;
    vec3  direction;
};

struct pointLight
{
    light base;
    vec3  position;
    float constant;
    float linear;
    float exponent;
};

struct spotLight
{
    pointLight base;
    vec3 direction;
    float edge;
};

//setting up surface properties
struct Material
{
    float specularIntensity;
    float specularPower;
};

//uniform variables for binding values with host
uniform int pointLightCount;
uniform int spotLightCount;

uniform vec3 FresnelCoef;

uniform DirectionalLight directionalLight;
uniform pointLight pointLights[MAX_POINT_LIGHTS];
uniform spotLight spotLights[MAX_SPOT_LIGHTS];

uniform RayMarching rayMarching;

uniform sampler3D theTex3DField;
uniform sampler3D theTex3DTexture;
uniform Material material0;

uniform vec3 eyePosition;
uniform vec3 cameraViewDir;

//main fragment part: functions, calculations, main

vec3 fresnelReflection( vec3 fresnelCoef, vec3 normal, vec3 lightPos )
{
    return fresnelCoef + ( 1.0f - fresnelCoef ) * pow( clamp( 1.0 - dot(normal, lightPos), 0.0f, 0.0f ), 5.0f );
}

vec3 calcNormal( vec3 curTex3DPos )
{
    vec3 shift = vec3( rayMarching.gradStep, 0.0f, 0.0f );

    float x = texture( theTex3DField, curTex3DPos + shift.xyy).x -
              texture( theTex3DField, curTex3DPos - shift.xyy).x;
    float y = texture( theTex3DField, curTex3DPos + shift.yxy).y -
              texture( theTex3DField, curTex3DPos - shift.yxy).y;
    float z = texture( theTex3DField, curTex3DPos + shift.yyx).z -
              texture( theTex3DField, curTex3DPos - shift.yyx).z;

    return normalize( vec3( x, y, z ));
}

vec4 calcLightByDirection( light baseLight, vec3 direction, vec3 normal )
{
    vec4 ambientColour  = vec4( baseLight.colour, 1.0f ) * baseLight.ambientIntensity;
    //max function checking whether we have angle below zero and if yes, clamp it to 0
    float diffuseFactor = max( dot( normalize( normal ), normalize( direction ) ), 0.0f);
    vec4  diffuseColour = vec4( baseLight.colour * baseLight.diffuseIntensity * diffuseFactor, 1.0f );

    vec4 specularColour = vec4( 0, 0, 0, 0 );
    if( diffuseFactor > 0.0f )
    {
        vec3 fragToEye       = normalize( eyePosition - FragPos );
        vec3 reflectedVertex = normalize( reflect( direction, normalize( normal )));
        float specularFactor = dot( fragToEye, reflectedVertex );

        if( specularFactor > 0.0f )
        {
            specularFactor = pow( specularFactor, material0.specularPower );
            specularColour = vec4( baseLight.colour * material0.specularIntensity * specularFactor, 1.0f );
        }
    }

    return ( ambientColour + diffuseColour + specularColour );
}

vec4 calcDirectionalLight( vec3 normal )
{
    return calcLightByDirection( directionalLight.base, directionalLight.direction, normal );
}

vec4 calcPointLight( pointLight pLight, vec3 normal )
{
    vec3 direction = FragPos - pLight.position;
    float distance = length( direction );
    direction = normalize( direction );

    vec4 colour = calcLightByDirection( pLight.base, direction, normal );
    float attenuation = pLight.exponent * distance * distance +
                        pLight.linear * distance +
                        pLight.constant;

    return ( colour / attenuation );
}

vec4 calcSpotLight( spotLight sLight, vec3 normal )
{
    vec3 rayDirection = FragPos - sLight.base.position;
    float slFactor    = dot( normalize(rayDirection), normalize(sLight.direction) );

    if( slFactor > sLight.edge )
    {
        vec4 colour = calcPointLight( sLight.base, normal );
        return colour * ( 1.0f - ( 1.0f - slFactor )*( 1.0f/( 1.0f - sLight.edge)));
    }
    else
    {
        return vec4( 0, 0, 0, 0 );
    }
}

vec4 calcPointLights( vec3 normal )
{
    vec4 totalColour = vec4( 0, 0, 0, 0 );
    for( int i = 0; i < pointLightCount; i++ )
    {
        totalColour += calcPointLight( pointLights[i], normal );
    }

    return totalColour;
}

vec4 calcSpotLights( vec3 normal )
{
    vec4 totalColour = vec4( 0, 0, 0, 0 );
    for( int i = 0; i < spotLightCount; i++ )
    {
        totalColour += calcSpotLight( spotLights[i], normal );
    }

    return totalColour;
}

bool RayMarching_CheckHit( vec3 rayOrigPos, vec3 rayDir, inout float depth, inout vec3 normal )
{
    float totalDistTravelled = 0.0;
    float minDistTravelled   = 0.0;
    float distToClosestPoint = 0.0;

    for( int i = 0; i < rayMarching.numberOfSteps; i++ )
    {
        vec3 curPos = rayOrigPos + totalDistTravelled * rayDir;
        distToClosestPoint = texture(theTex3DField, curPos).x;

        if( distToClosestPoint < rayMarching.minHitDist )
        {
            break;
        }

        minDistTravelled    = min( abs( distToClosestPoint ), 0.1 );
        totalDistTravelled += minDistTravelled;

        if( totalDistTravelled > depth )
        {
            break;
        }
    }

    if( distToClosestPoint >= rayMarching.minHitDist )
    {
        return false;
    }

    totalDistTravelled -= minDistTravelled;
    for( int i = 0; i < 4; i++ )
    {
        minDistTravelled *= 0.5;
        vec3 curPos = rayOrigPos + ( totalDistTravelled + minDistTravelled ) * rayDir;
        if( texture(theTex3DField, curPos).x >= rayMarching.minHitDist )
        {
            totalDistTravelled += minDistTravelled;
        }
    }

    depth = totalDistTravelled;
    normal = calcNormal( rayOrigPos + rayDir * totalDistTravelled );
    return true;
}

void main()
{
    float depth = rayMarching.clipFar;
    vec3 normal = vec3( 0.0f, 0.0f, 0.0f );

    if( !RayMarching_CheckHit( eyePosition, cameraViewDir, depth, normal ) )
    {
        colour = vec4( 1, 1, 1, 1 );
        return;
    }

    vec3 objPos = eyePosition + cameraViewDir*depth;
    vec4 finalColour = calcDirectionalLight( normal );
    finalColour += calcPointLights( normal );
    finalColour += calcSpotLights( normal);

    colour = finalColour;
}
