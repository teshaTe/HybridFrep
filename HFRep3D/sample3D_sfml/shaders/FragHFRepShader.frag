#version 330 core

out vec4 colour;

in vec3 FragPos;
in vec3 texPos;

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
uniform samplerCube skyboxSampler;

float gradStep  = 0.00001;
float normCoeff = 10.0;
float Pi = 3.14159265359;

//------------------------------------------------------------------------------
//ray marching and shadow mapping
//------------------------------------------------------------------------------


float sdPlane(in vec3 p) {
    return (-p + vec3(0.0, -0.3, 0.0)).y;
}

vec3 groundNormal(in vec3 position)
{
    vec3 epsilon = vec3(0.001, 0.0, 0.0);
    vec3 n = vec3(
          sdPlane(position + epsilon.xyy) - sdPlane(position - epsilon.xyy),
          sdPlane(position + epsilon.yxy) - sdPlane(position - epsilon.yxy),
          sdPlane(position + epsilon.yyx) - sdPlane(position - epsilon.yyx));
    return normalize(n);
}

vec3 worldToTextureCoords( vec3 pos )
{
    vec3 size   = AABB_max - AABB_min;
    vec3 mixVal = ( pos - AABB_min ) / size;
    return mixVal;
}

vec3 obtainNormal( vec3 uvwPos )
{
    vec3 n_shift = vec3( worldToTextureCoords( vec3(gradStep) ).x, 0.0, 0.0 );

    float x = texture( HFRepField, uvwPos - n_shift.xyy ).r - texture( HFRepField, uvwPos + n_shift.xyy ).r;
    float y = texture( HFRepField, uvwPos - n_shift.yxy ).r - texture( HFRepField, uvwPos + n_shift.yxy ).r;
    float z = texture( HFRepField, uvwPos - n_shift.yyx ).r - texture( HFRepField, uvwPos + n_shift.yyx ).r;

    return normalize( vec3( x, y, z ) );
}

float shadowMapping( vec3 rayOrigPos, vec3 rayDir, float objID )
{
    float hit = 1.0;
    float totalDistTravelled;
    float distToClosestPoint = 0.0;
    vec3 curPos = vec3(0.0);

    if( objID == 1.0 )
        totalDistTravelled = 0.02;
    else
        totalDistTravelled = 0.5;

    for(int i = 0; i < rayMarching.numberOfSteps; i++)
    {
        if( objID == 1.0 )
        {
            curPos = rayOrigPos + totalDistTravelled * rayDir;
            distToClosestPoint = sdPlane( curPos );
        }
        else
        {
            curPos = worldToTextureCoords(rayOrigPos + totalDistTravelled * rayDir);;
            distToClosestPoint = texture( HFRepField, curPos ).r / normCoeff;
        }

        if( distToClosestPoint > rayMarching.minHitDist )
            return 0.0;

        totalDistTravelled += abs(distToClosestPoint);
        hit = min( hit, 10.0 * abs(distToClosestPoint) / totalDistTravelled );

        if( totalDistTravelled > rayMarching.maxHitDist )
            break;
    }

    return clamp( hit, 0.0, 1.0 );
}

bool RayMarching_CheckHit( vec3 rayOrigPos, vec3 rayDir, out vec3 normal, out float depth, float objID )
{
    float totalDistTravelled;

    if( objID == 1.0 )
        totalDistTravelled = 0.02;
    else
        totalDistTravelled = 0.5;

    float distToClosestPoint = 0.0;
    vec3 curPos = vec3(0.0);

    for( int i = 0; i < rayMarching.numberOfSteps; i++ )
    {
        if( objID == 1.0 )
        {
            curPos = rayOrigPos + totalDistTravelled * rayDir;
            distToClosestPoint = sdPlane( curPos );
        }
        else
        {
            curPos = worldToTextureCoords(rayOrigPos + totalDistTravelled * rayDir);
            distToClosestPoint = texture( HFRepField, curPos ).r / normCoeff;
        }

        if( distToClosestPoint > rayMarching.minHitDist )
        {
            if (objID == 1.0)
                normal = groundNormal( curPos );
            else
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

//------------------------------------------------------------------------------
//BRDF
// v - outcoming direction of the light;
// h - bisector vector of V and V'; V  is the direction of the leaving radiance from point P;
//                                  V' is the direction of the incident radiance to the point P;
// l - light direction
//------------------------------------------------------------------------------

float pow5( float val ) { return val*val*val*val*val; }

//GGX microfacet distribution function
float DGGX_refraction( float linearRoughness, float N_Dot_H, const vec3 h )
{
    float delta = 1.0 - N_Dot_H*N_Dot_H;
    float a = linearRoughness * N_Dot_H;
    float b = linearRoughness / ( delta + a*a );
    float D = b*b * (1.0 / Pi );
    return D;
}

//Smith correlated masking-shadowing function
float maskingShadowingGGX( float linearRoughness, float N_Dot_V, float N_Dot_L )
{
    float alpha2 = linearRoughness * linearRoughness;
    float GGXV = N_Dot_L * sqrt(( N_Dot_V - alpha2*N_Dot_V ) * N_Dot_V + alpha2 );
    float GGXL = N_Dot_V * sqrt(( N_Dot_L - alpha2*N_Dot_L ) * N_Dot_L + alpha2 );
    return 0.5 / ( GGXV + GGXL );
}

//Shlick BRDF model for Fresnel coefficient
vec3 Fresnel_Schlick( const vec3 fLamb, float V_Dot_H )
{
    return fLamb + ( vec3(1.0) - fLamb ) * pow5( 1.0 - V_Dot_H );
}

float Fresnel_Schlick( float fLamb, float f90, float V_Dot_H)
{
    return fLamb + (f90 - fLamb) * pow5(1.0 - V_Dot_H);
}

//diffuse model for the specular (Burley)
float FDiffuse_Burley( float linearRoughness, float L_Dot_H, float N_Dot_V, float N_Dot_L )
{
    float f90 = 0.5 + 2.0 * linearRoughness * L_Dot_H * L_Dot_H;
    float lightScatter = Fresnel_Schlick( 1.0, f90, N_Dot_L );
    float viewScatter  = Fresnel_Schlick( 1.0, f90, N_Dot_V );
    return lightScatter * viewScatter * (1.0 / Pi);
}

//lambert diffuse model
float Fd_Lambert()
{
    return 1.0 / Pi;
}

//------------------------------------------------------------------------------
// Indirect lighting
//------------------------------------------------------------------------------

vec3 Irradiance_SphericalHarmonics(const vec3 n)
{
    // Irradiance from "Ditch River" IBL (http://www.hdrlabs.com/sibl/archive.html)
    return max(
          vec3( 0.754554516862612,  0.748542953903366,  0.790921515418539)
        + vec3(-0.083856548007422,  0.092533500963210,  0.322764661032516) * (n.y)
        + vec3( 0.308152705331738,  0.366796330467391,  0.466698181299906) * (n.z)
        + vec3(-0.188884931542396, -0.277402551592231, -0.377844212327557) * (n.x)
        , 0.0);
}

vec2 PrefilteredDFG_Karis(float roughness, float N_Dot_V)
{
    // Karis 2014, "Physically Based Material on Mobile"
    const vec4 c0 = vec4(-1.0, -0.0275, -0.572,  0.022);
    const vec4 c1 = vec4( 1.0,  0.0425,  1.040, -0.040);

    vec4 r = roughness * c0 + c1;
    float a004 = min(r.x * r.x, exp2(-9.28 * N_Dot_V)) * r.x + r.y;

    return vec2(-1.04, 1.04) * a004 + r.zw;
}

//------------------------------------------------------------------------------
// Tone mapping and transfer functions
//------------------------------------------------------------------------------

vec3 Tonemap_ACES(const vec3 x)
{
    // Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return (x * (a * x + b)) / (x * (c * x + d) + e);
}

vec3 OECF_sRGBFast(const vec3 linear)
{
    return pow(linear, vec3(1.0 / 2.2));
}

//------------------------------------------------------------------------------
//Rendering
//------------------------------------------------------------------------------

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

void shading( vec3 rayOrigin, vec3 direction, float objID, inout float distance, inout vec3 color )
{
    vec3 position = rayOrigin + distance * direction;
    vec3 Vv = normalize( -direction );
    vec3 N = vec3(0.0);

    if ( objID == 1.0 )
        N  = groundNormal( position );
    else
        N  = obtainNormal( worldToTextureCoords( position ));

    //vec3 L  = normalize( vec3( 0.6, 0.7, -0.7));
    vec3 L  = normalize( directionalLight.direction );
    vec3 H  = normalize( Vv + L );
    vec3 R  = normalize( reflect( direction, N ));

    float N_Dot_V = abs( dot( N, Vv )) + 1e-5;
    float N_Dot_L = clamp( dot( N, L ), 0.0, 1.0 );
    float N_Dot_H = clamp( dot( N, H ), 0.0, 1.0 );
    float L_Dot_H = clamp( dot( L, H ), 0.0, 1.0 );

    //TAKE THIS AWAY AND MAKE AS INPUT
    vec3 baseColor  = vec3(0.0);
    float roughness = 0.0;
    float metallic  = 0.0;

    float intensity = 3.0;
    float indirectIntensity = 0.64;
    //*************************************

    //object
    if(objID == 1.0)
    {
        float f = mod(floor(6.0 * position.z) + floor(6.0 * position.x), 2.0);
        baseColor = 0.4 + f * vec3(0.6);
        roughness = 0.1;
    }
    else
    {
        baseColor = vec3( 0.3, 0.0, 0.0);
        roughness = 0.2;
    }

    float linearRoughness = roughness * roughness;
    float attenuation     = shadowMapping( position, L, objID ) ;
    vec3 diffuseColor     = ( 1.0 - metallic ) * baseColor.rgb;
    vec3 fLambda          = 0.04 * ( 1.0 - metallic ) + baseColor.rgb * metallic;

    //specular BRDF
    float D  = DGGX_refraction( linearRoughness, N_Dot_H, H );
    float V  = maskingShadowingGGX( linearRoughness, N_Dot_V, N_Dot_L );
    vec3 F   = Fresnel_Schlick( fLambda, L_Dot_H );
    vec3 Fr  = ( D * V ) * F;

    //diffuse BRDF
    vec3 Fd = diffuseColor * FDiffuse_Burley( linearRoughness, L_Dot_H, N_Dot_V, N_Dot_L );
    color  = Fd + Fr;
    color *= ( intensity * attenuation * N_Dot_L ) * vec3(0.98, 0.92, 0.89);

    //diffuse indirect
    vec3 indirectDiffuse  = Irradiance_SphericalHarmonics( N ) * Fd_Lambert();
    vec3 indirectSpecular = texture(skyboxSampler, R).rgb;

    float distance0;
    vec3 normal0;

    if( RayMarching_CheckHit( position, R, normal0, distance0, objID ))
    {
        if( objID == 1.0 )
        {
            vec3 indirectPosition = position + distance0 * R;
            // Checkerboard floor
            float f = mod(floor(6.0 * indirectPosition.z) + floor(6.0 * indirectPosition.x), 2.0);
            indirectSpecular = f * texture(skyboxSampler, R).rgb;
        }
        else
        {
            indirectSpecular = vec3( 0.3, 0.0, 0.0 );
        }
    }

    //indirect contribution
    vec2 dfg = PrefilteredDFG_Karis( roughness, N_Dot_V );
    vec3 specularColor = fLambda * dfg.x + dfg.y;
    vec3 ibl = diffuseColor * indirectDiffuse + indirectSpecular * specularColor;

    color += ibl * indirectIntensity;
}

void rendering( vec3 rayOrigin, vec3 direction, vec3 normal, out float distance, out vec3 color )
{
    if( RayMarching_CheckHit( rayOrigin, direction, normal, distance, 2.0 ) )
    {
        shading( rayOrigin, direction, 2.0, distance, color);
    }
    /*else if( RayMarching_CheckHit( rayOrigin, direction, normal, distance, 1.0 ) )
    {
        shading( rayOrigin, direction, 1.0, distance, color);
    }*/
    else
        discard;
}

void main()
{
    vec3 cameraViewDir = normalize( FragPos - eyePosition );
    vec2 T;
    vec3 normal = vec3(0.0);
    float depth;

    if( intersect_vs_aabb( eyePosition, cameraViewDir, AABB_min, AABB_max, T ) )
    {
        vec3 enter_pos = eyePosition + T.x * cameraViewDir;
        vec3 exit_pos  = eyePosition + T.y * cameraViewDir;
        vec3 viewDir   = normalize( exit_pos - enter_pos );

        vec3 color = vec3(0.0);
        rendering( enter_pos, viewDir, normal, depth, color );

        color = Tonemap_ACES( color );
        color = mix(color, 0.8 * vec3(0.7, 0.8, 1.0), 1.0 - exp2(-0.011 * depth * depth));
        color = OECF_sRGBFast( color );
        colour = vec4( color, 1.0 );
    }
    else
        discard;
}
