#version 330 core

out vec4 colour;

in vec3 FragPos;

struct RayMarching
{
    int   numberOfSteps;
    float gradStep;
    float minHitDist;
    float maxHitDist;
};

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

struct Material
{
    float fresnelCoeff;
    float specularIntensity;
    float specularPower;
};

uniform vec3 AABB_min;
uniform vec3 AABB_max;
uniform vec3 eyePosition;
uniform vec2 HFRepMinMax;

uniform RayMarching rayMarching;
uniform DirectionalLight directionalLight;
uniform Material material;

uniform sampler3D HFRepField;
uniform sampler3D normalMap;

float gradStep  = 0.05;
float normCoeff = 10.0;

vec3 worldToTextureCoords( vec3 pos )
{
    vec3 size   = AABB_max - AABB_min;
    vec3 mixVal = ( pos - AABB_min ) / size;
    return mixVal;
}

vec3 obtainNormal( vec3 uvwPos )
{
    vec3 shift   = vec3( gradStep, 0.0, 0.0 );
    vec3 n_shift = vec3( worldToTextureCoords( shift ).x, 0.0, 0.0 );

    float x = texture( HFRepField, uvwPos - n_shift.xyy ).r - texture( HFRepField, uvwPos + n_shift.xyy ).r;
    float y = texture( HFRepField, uvwPos - n_shift.yxy ).r - texture( HFRepField, uvwPos + n_shift.yxy ).r;
    float z = texture( HFRepField, uvwPos - n_shift.yyx ).r - texture( HFRepField, uvwPos + n_shift.yyx ).r;

    return normalize( vec3( x, y, z ) );
}

float fresnelReflection( vec3 normal, vec3 lightPos )
{
    return material.fresnelCoeff + ( 1.0 - material.fresnelCoeff ) *
           pow( clamp( 1.0 - dot( normal, lightPos ), 0.0, 1.0 ), 5.0 );
}

vec4 calcLightByDirection( light baseLight, vec3 direction, vec3 normal, vec3 objPos, vec3 eyePos )
{
    vec4 ambientColour  = vec4( baseLight.colour, 1.0f ) * baseLight.ambientIntensity;

    //max function checking whether we have angle below zero and if yes, clamp it to 0
    float diffuseFactor = max( dot( normal, normalize( direction ) ), 0.0f);
    vec4  diffuseColour = vec4( baseLight.colour * baseLight.diffuseIntensity * diffuseFactor, 1.0f );

    vec4 specularColour = vec4( 0, 0, 0, 0 );
    if( diffuseFactor > 0.0f )
    {
        vec3 fragToEye       = normalize( eyePos - objPos );
        vec3 reflectedVertex = normalize( reflect( direction, normal ));
        float specularFactor = dot( fragToEye, reflectedVertex );

        if( specularFactor > 0.0f )
        {
            specularFactor = pow( specularFactor, material.specularPower );
            specularColour = vec4( baseLight.colour * material.specularIntensity * specularFactor, 1.0f );
        }
    }

    return ( ambientColour + diffuseColour + specularColour );
}

vec4 calcDirectionalLight( vec3 normal, vec3 objPos, vec3 eyePos )
{
    return calcLightByDirection( directionalLight.base, directionalLight.direction, normal, objPos, eyePos );
}

bool RayMarching_CheckHit( vec3 rayOrigPos, vec3 rayDir, out vec3 normal, out float depth)
{
    float totalDistTravelled = 0.0f;
    float minDistTravelled   = 0.0f;
    float distToClosestPoint = 0.0f;

    for( int i = 0; i < rayMarching.numberOfSteps; i++ )
    {
        vec3 curPos = rayOrigPos + totalDistTravelled * rayDir;
        distToClosestPoint = texture( HFRepField, curPos ).r / normCoeff;

        if( distToClosestPoint > rayMarching.minHitDist )
        {
            normal = obtainNormal( curPos );
            depth  = totalDistTravelled;
            return true;
        }

        if( totalDistTravelled > rayMarching.maxHitDist )
        {
            break;
        }

        totalDistTravelled += abs(distToClosestPoint);
    }

    return false;
}

bool intersect_vs_aabb( vec3 rayOrig, vec3 dir, vec3 aabb_min, vec3 aabb_max, out vec2 T )
{
    vec3 tbot = (aabb_min - rayOrig) / dir;
    vec3 ttop = (aabb_max - rayOrig) / dir;

    vec3 tmin = min(tbot, ttop);
    vec3 tmax = max(tbot, ttop);

    float tnear = max( max(tmin.x, tmin.y), max(tmin.x, tmin.z));
    float tfar  = min( min(tmax.x, tmax.y), min(tmax.x, tmax.z));

    T = vec2(tnear, tfar);

    return tfar > tnear;
}

void main()
{
    vec3 cameraViewDir = normalize( FragPos - eyePosition );
    vec2 T;
    vec3 normal;
    float depth;

    if( intersect_vs_aabb( eyePosition, cameraViewDir, AABB_min, AABB_max, T ) )
    {
        vec3 enter_pos = eyePosition + T.x * cameraViewDir;
        vec3 exit_pos  = eyePosition + T.y * cameraViewDir;
        vec3 viewDir   = normalize( worldToTextureCoords(exit_pos) - worldToTextureCoords(enter_pos) );
        //vec3 viewDir = normalize( exit_pos - enter_pos );

        if( RayMarching_CheckHit( worldToTextureCoords(enter_pos), viewDir, normal, depth ) )
        {
            vec3 objPos   = enter_pos + depth * normalize( exit_pos - enter_pos );
            vec4 dirLight = calcDirectionalLight( normal, objPos, enter_pos );
            colour = vec4(1.0, 0.0, 0.0, 1.0) * dirLight;
        }
        else
            discard;
    }
    else
        discard;

}
