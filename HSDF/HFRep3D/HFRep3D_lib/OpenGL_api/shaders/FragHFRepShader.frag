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
    float startTraceDist;
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

uniform float cutPlanePosY;
uniform int isolines;

uniform RayMarching rayMarching;
uniform DirectionalLight directionalLight;
uniform Material material;

uniform sampler3D HFRepField;
uniform samplerCube skyboxSampler;

float gradStep  = 0.001;
float Pi = 3.14159265359;

//float distTr = 0.01;

//------------------------------------------------------------------------------
//ray marching and shadow mapping
//------------------------------------------------------------------------------
vec3 worldToTextureCoords(vec3 pos)
{
    vec3 size   = AABB_max - AABB_min;
    vec3 mixVal = (pos - AABB_min) / size;
    return mixVal;
}

vec3 textureCoordsToWorld(vec3 pos)
{
    vec3 size   = AABB_max - AABB_min;
    vec3 mixVal = pos * size + AABB_min;
    return mixVal;
}

vec3 obtainNormal(vec3 posWorld)
{
    vec3 n_shift = vec3(worldToTextureCoords(vec3(gradStep)).x, 0.0, 0.0);
    vec3 uvwPos = worldToTextureCoords(posWorld);

    float x = (texture(HFRepField, uvwPos - n_shift.xyy).r - texture(HFRepField, uvwPos + n_shift.xyy).r);
    float y = (texture(HFRepField, uvwPos - n_shift.yxy).r - texture(HFRepField, uvwPos + n_shift.yxy).r);
    float z = (texture(HFRepField, uvwPos - n_shift.yyx).r - texture(HFRepField, uvwPos + n_shift.yyx).r);
    return normalize(vec3(x, y, z));
}

//new part ****************************************************************************
float map (in vec3 p)
{
    vec3 uvwPos = worldToTextureCoords(p);
    vec3 mPos = worldToTextureCoords(vec3(0, cutPlanePosY, 0));
    return max(texture(HFRepField, uvwPos).r, -(p.y + 2. * (2. * (1. - mPos.y) - 1.)));
}

vec3 obtainNormalMap(vec3 posWorld)
{
    //vec3 n_shift = vec3(gradStep, 0.0, 0.0);
    vec3 n_shift = vec3(worldToTextureCoords(vec3(gradStep)).x, 0.0, 0.0);
    vec3 uvwPos = worldToTextureCoords(posWorld);
    vec3 mPos = worldToTextureCoords(vec3(0, cutPlanePosY, 0));

    vec3 p11 = posWorld - n_shift.xyy;
    vec3 p12 = posWorld + n_shift.xyy;
    float x1 = max(texture(HFRepField, uvwPos - n_shift.xyy).r, -(p11.y + 2. * (2. * (1. - mPos.y) - 1.)));
    float x2 = max(texture(HFRepField, uvwPos + n_shift.xyy).r, -(p12.y + 2. * (2. * (1. - mPos.y) - 1.)));

    vec3 p21 = posWorld - n_shift.yxy;
    vec3 p22 = posWorld + n_shift.yxy;
    float y1 = max(texture(HFRepField, uvwPos - n_shift.yxy).r, -(p21.y + 2. * (2. * (1. - mPos.y) - 1.)));
    float y2 = max(texture(HFRepField, uvwPos + n_shift.yxy).r, -(p22.y + 2. * (2. * (1. - mPos.y) - 1.)));

    vec3 p31 = posWorld - n_shift.yyx;
    vec3 p32 = posWorld + n_shift.yyx;
    float z1 = max(texture(HFRepField, uvwPos - n_shift.yyx).r, -(p31.y + 2. * (2. * (1. - mPos.y) - 1.)));
    float z2 = max(texture(HFRepField, uvwPos + n_shift.yyx).r, -(p32.y + 2. * (2. * (1. - mPos.y) - 1.)));

    return normalize(vec3(x1 - x2, y1 - y2, z1 - z2));
}
//***************************************************************************************
float shadowMapping(vec3 rayOrigPos, vec3 rayDir)
{
    float hit = 1.0;
    float totalDistTravelled = rayMarching.startTraceDist;
    float distToClosestPoint = 0.0;
    vec3 curPosUVW = vec3(0.0), curPos = vec3(0.0);

    bool iso = false;

    for(int i = 0; i < rayMarching.numberOfSteps; i++)
    {
        curPos = rayOrigPos + totalDistTravelled * rayDir;
        curPosUVW = worldToTextureCoords(curPos);
        if(isolines == 0)
            distToClosestPoint = texture(HFRepField, curPosUVW).r ;
        else
            distToClosestPoint = map(curPos);

        if(distToClosestPoint > rayMarching.minHitDist &&
            totalDistTravelled < rayMarching.maxHitDist)
            return 0.0;

        totalDistTravelled += abs(distToClosestPoint);
        hit = min(hit, 10.0 * abs(distToClosestPoint) / totalDistTravelled);

        if(totalDistTravelled > rayMarching.maxHitDist)
            break;
    }

    return clamp(hit, 0.0, 1.0);
}

bool RayMarching_CheckHit(vec3 rayOrigPos, vec3 rayDir, out vec3 normal, out float depth)
{
    float totalDistTravelled = rayMarching.startTraceDist;
    float distToClosestPoint = 0.0;
    vec3 curPos = vec3(0.0), curPosUVW = vec3(0.0);

    for(int i = 0; i < rayMarching.numberOfSteps; i++)
    {
        curPos = rayOrigPos + totalDistTravelled * rayDir;
        curPosUVW = worldToTextureCoords(curPos);
        if(isolines == 0)
            distToClosestPoint = texture(HFRepField, curPosUVW).r ;
        else
            distToClosestPoint = map(curPos);

        if(distToClosestPoint > rayMarching.minHitDist &&
            totalDistTravelled < rayMarching.maxHitDist)
        {
            if(isolines == 0)
                normal = obtainNormal(curPos);
            else
                normal = obtainNormalMap(curPos);
            depth  = totalDistTravelled;
            return true;
        }

        if(totalDistTravelled > rayMarching.maxHitDist)
            break;

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

float pow5(float val) { return val*val*val*val*val; }

//GGX microfacet distribution function
float DGGX_refraction(float linearRoughness, float N_Dot_H, const vec3 h)
{
    float delta = 1.0 - N_Dot_H*N_Dot_H;
    float a = linearRoughness * N_Dot_H;
    float b = linearRoughness / (delta + a*a);
    float D = b*b * (1.0 / Pi);
    return D;
}

//Smith correlated masking-shadowing function
float maskingShadowingGGX(float linearRoughness, float N_Dot_V, float N_Dot_L)
{
    float alpha2 = linearRoughness * linearRoughness;
    float GGXV = N_Dot_L * sqrt((N_Dot_V - alpha2*N_Dot_V) * N_Dot_V + alpha2);
    float GGXL = N_Dot_V * sqrt((N_Dot_L - alpha2*N_Dot_L) * N_Dot_L + alpha2);
    return 0.5 / (GGXV + GGXL);
}

//Shlick BRDF model for Fresnel coefficient
vec3 Fresnel_Schlick(const vec3 fLamb, float V_Dot_H)
{
    return fLamb + (vec3(1.0) - fLamb) * pow5(1.0 - V_Dot_H);
}

float Fresnel_Schlick(float fLamb, float f90, float V_Dot_H)
{
    return fLamb + (f90 - fLamb) * pow5(1.0 - V_Dot_H);
}

//diffuse model for the specular (Burley)
float FDiffuse_Burley(float linearRoughness, float L_Dot_H, float N_Dot_V, float N_Dot_L)
{
    float f90 = 0.5 + 2.0 * linearRoughness * L_Dot_H * L_Dot_H;
    float lightScatter = Fresnel_Schlick(1.0, f90, N_Dot_L);
    float viewScatter  = Fresnel_Schlick(1.0, f90, N_Dot_V);
    return lightScatter * viewScatter * (1.0 / Pi);
}

//lambert diffuse model
float Fd_Lambert() { return 1.0 / Pi; }

//------------------------------------------------------------------------------
// Indirect lighting
//------------------------------------------------------------------------------

vec3 Irradiance_SphericalHarmonics(const vec3 n)
{
    // Irradiance from "Ditch River" IBL (http://www.hdrlabs.com/sibl/archive.html)
    return max(vec3(0.754554516862612,  0.748542953903366,  0.790921515418539)
              + vec3(-0.083856548007422,  0.092533500963210,  0.322764661032516) * (n.y)
              + vec3(0.308152705331738,  0.366796330467391,  0.466698181299906) * (n.z)
              + vec3(-0.188884931542396, -0.277402551592231, -0.377844212327557) * (n.x)
              , 0.0);
}

vec2 PrefilteredDFG_Karis(float roughness, float N_Dot_V)
{
    // Karis 2014, "Physically Based Material on Mobile"
    const vec4 c0 = vec4(-1.0, -0.0275, -0.572,  0.022);
    const vec4 c1 = vec4(1.0,  0.0425,  1.040, -0.040);

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

vec3 OECF_sRGBFast(const vec3 linear) { return pow(linear, vec3(1.0 / 2.2)); }

//------------------------------------------------------------------------------
//Rendering
//------------------------------------------------------------------------------

bool intersect_vs_aabb(vec3 rayOrig, vec3 dir, vec3 aabb_min, vec3 aabb_max, out vec2 T)
{
    vec3 tbot = (aabb_min - rayOrig) / dir;
    vec3 ttop = (aabb_max - rayOrig) / dir;

    vec3 tmin = min(tbot, ttop);
    vec3 tmax = max(tbot, ttop);

    float tnear = max(max(tmin.x, tmin.y), max(tmin.x, tmin.z));
    float tfar  = min(min(tmax.x, tmax.y), min(tmax.x, tmax.z));

    T = vec2(tnear, tfar);

    return tfar > tnear;
}

vec2 cwise_mult(vec2 a, vec2 b)
{
    return vec2(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x);
}

vec2 cwise_sqrt(vec2 a)
{
    return vec2(a.x*a.x - a.y*a.y, 2.0*a.x*a.y);
}

float map_material(in vec3 p)
{
    float res = 0;
    vec3 c = p;

    for(int i = 0; i < 10; i++)
    {
        p = 0.7 * abs(p) / dot(p, p) - 0.7;
        p.yz = cwise_sqrt(p.yz);
        p = p.zxy;
        res += exp(-19.0 * abs(dot(p, c)));
    }
    return res / 2.0;
}

vec3 shade_marble(in vec3 ro, vec3 rd, vec2 tminmax)
{
    float t = rayMarching.startTraceDist;
    //float dt = 0.02;
    float dt = 0.2 - 0.195*cos(0.09);
    vec3 col = vec3(0.0);
    float c = 0;

    for(int i = 0; i < 64; i++)
    {
        t += dt * exp(-2.0*c);
        if(t > tminmax.y)
            break;
        vec3 pos = ro + t*rd;
        c = map_material(pos);
        //col = 0.99*col + 0.08 * vec3(c*c, c, c*c*c);
        col = 0.99*col + 0.08 * vec3(c*c*c, c*c, c);
    }
    return col;
}

void shading(vec3 rayOrigin, vec3 direction, inout float distance, inout vec3 color)
{
    vec3 position = rayOrigin + distance * direction;
    vec3 Vv = normalize(-direction);
    vec3 N;
    if(isolines == 0)
        N = obtainNormal(position);
    else
        N = obtainNormalMap(position);

    vec3 L  = normalize(directionalLight.direction);
    vec3 H  = normalize(Vv + L);
    vec3 R  = normalize(reflect(direction, N));

    float N_Dot_V = abs(dot(N, Vv)) + 1e-5;
    float N_Dot_L = clamp(dot(N, L), 0.0, 1.0);
    float N_Dot_H = clamp(dot(N, H), 0.0, 1.0);
    float L_Dot_H = clamp(dot(L, H), 0.0, 1.0);

    //TAKE THIS AWAY AND MAKE AS INPUT
    vec3 baseColor  = vec3(0.0);
    float roughness = 0.0;
    float metallic  = 0.0;

    float intensity = 3.0;
    float indirectIntensity = 0.64;

    if(isolines == 0)
    {
        //baseColor = shade_marble(rayOrigin, direction, vec2(0, 0.08));
        baseColor = vec3(1, 0, 0);
        roughness = 0.2;
    }
    else
    {
        float mask = smoothstep(1.0, 0.05, 30.0*cos (100.0*position.y)+sin (100.0*position.x)+ cos (100.0*position.z));
        baseColor = mix(vec3 (0.5), vec3 (0.2), mask);
        metallic = 0.5;
        roughness = mix (0.45, 0.175, mask);
    }

    float linearRoughness = roughness * roughness;
    float attenuation     = shadowMapping(position, L) ;
    vec3 diffuseColor     = (1.0 - metallic) * baseColor.rgb;
    vec3 fLambda          = 0.04 * (1.0 - metallic) + baseColor.rgb * metallic;

    //specular BRDF
    float D  = DGGX_refraction(linearRoughness, N_Dot_H, H);
    float V  = maskingShadowingGGX(linearRoughness, N_Dot_V, N_Dot_L);
    vec3 F   = Fresnel_Schlick(fLambda, L_Dot_H);
    vec3 Fr  = (D * V) * F;

    //diffuse BRDF
    vec3 Fd = diffuseColor * FDiffuse_Burley(linearRoughness, L_Dot_H, N_Dot_V, N_Dot_L);
    color  = Fd + Fr;
    color *= (intensity * attenuation * N_Dot_L) * vec3(0.98, 0.92, 0.89);

    //diffuse indirect
    vec3 indirectDiffuse  = Irradiance_SphericalHarmonics(N) * Fd_Lambert();
    vec3 indirectSpecular;

    vec3 normal1; float distance1;
    if(RayMarching_CheckHit(position, R, normal1, distance1))
        indirectSpecular = baseColor;
    else
        indirectSpecular = texture(skyboxSampler, R).rgb;

    //indirect contribution
    vec2 dfg = PrefilteredDFG_Karis(roughness, N_Dot_V);
    vec3 specularColor = fLambda * dfg.x + dfg.y;
    vec3 ibl = diffuseColor * indirectDiffuse + indirectSpecular * specularColor;

    color += ibl * indirectIntensity;
}

void rendering(vec3 rayOrigin, vec3 direction, vec3 normal, out float distance, out vec3 color)
{
    if(RayMarching_CheckHit(rayOrigin, direction, normal, distance))
    {
        shading(rayOrigin, direction, distance, color);
    }
    else
        discard;
}

float saturate (in float v) { return clamp (v, .0, 1.); }

void main()
{
    vec3 cameraViewDir = normalize(FragPos - eyePosition);
    vec2 T;
    vec3 normal = vec3(0.0);
    float dist;

    if(intersect_vs_aabb(eyePosition, cameraViewDir, AABB_min, AABB_max, T))
    {
        vec3 enter_pos = eyePosition + T.x * cameraViewDir;
        vec3 exit_pos  = eyePosition + T.y * cameraViewDir;
        vec3 viewDir   = normalize(exit_pos - enter_pos);

        vec3 color = vec3(0.0);
        rendering(enter_pos, viewDir, normal, dist, color);

        if(isolines == 1)
        {
            // painting the isolines
            float isoLinesVal = texture(HFRepField, worldToTextureCoords(enter_pos + dist*viewDir)).r ;
            float density = 50.0;
            float thickness = 100.0;
            if (isoLinesVal <= 0.001f && isoLinesVal >= -0.10f) //-0.15f
            {
                color = mix(color, vec3 (0.1, 0.2, 0.5), pow(1.0 - 1.0 / dist, 5.0));
                color.rgb *= saturate(abs(fract(isoLinesVal*density)*2.0 - 1.0)*thickness/(dist*dist));
                color = Tonemap_ACES(color);
                color = mix(color, 0.8 * vec3(0.7, 0.8, 1.0), 1.0 - exp2(-0.011 * dist * dist));
                color = OECF_sRGBFast(color);
                colour = vec4(color, 1.0);
            }
            else
            {
                discard;
            }
        }
        else
        {
            color = Tonemap_ACES(color);
            color = mix(color, 0.8 * vec3(0.7, 0.8, 1.0), 1.0 - exp2(-0.011 * dist * dist));
            color = OECF_sRGBFast(color);
            colour = vec4(color, 1.0);
        }
    }
    else
        discard;
}
