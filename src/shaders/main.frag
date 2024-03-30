// Very basic shaded rotating cubeconst

#version 460

layout (location=0) uniform vec4 params;
layout (binding=0) uniform sampler2D previousFrame;

out vec4 outCol;

// constants
float SQRT32 = 0.86602540378;
float SQRT3 = 1.73205080757;
float PI = 3.1415926535;

// hex grid settings
float RI = 1.;
float RC = 1.15470053838; // RI/SQRT32;


vec2 sincos(float x){
    return vec2(sin(x), cos(x));
}

mat2 rot(float a){
    /*float c = cos(a);
    float s = sin(a);*/
    vec2 sc = sincos(a);
    return mat2(sc.y, -sc.x, sc.x, sc.y);
}



// from: https://www.shadertoy.com/view/4djSRW
float hash13(vec3 p3) {
    p3  = fract(p3 * .1031);
    p3 += dot(p3, p3.zyx + 31.32);
    return fract((p3.x + p3.y) * p3.z);
}

/*float hash12(vec2 p) {
    vec3 p3  = fract(vec3(p.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}*/

vec3 hash32(vec2 p)
{
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yxz+33.33);
    return fract((p3.xxy+p3.yzz)*p3.zyx);
}

/*float hash11(float p) {
    p = fract(p * .1031);
    p *= p + 33.33;
    p *= p + p;
    return fract(p);
}*/


float noise(vec3 p) {
    vec3 i = floor(p);
    vec3 w = smoothstep(0., 1., p-i);
    
    vec2 k = vec2(1., 0.);
    return 
    mix(mix(mix(hash13(i+k.yyy),
                hash13(i+k.xyy), w.x),
            mix(hash13(i+k.yxy),
                hash13(i+k.xxy), w.x), w.y),
        mix(mix(hash13(i+k.yyx),
                hash13(i+k.xyx), w.x),
            mix(hash13(i+k.yxx),
                hash13(i+k.xxx), w.x), w.y), w.z);
}

float fbm(vec3 p){
    float f = 0.;
    float s = 0.5;
    for(int i = 0; i < 8; i++) {
        f += s*noise(p);
        p *= 2.;
        s *= 0.6;
    }
    return f;
}


// xy = center of the hexagon, zw = coordinates relative to hexagon center
vec4 hexCell(vec2 p){
    vec2 s = vec2(1., SQRT3)*(2.*RI);
    vec2 h = s*0.5;
    vec2 a = mod(p, s) - h;
    vec2 b = mod(p + h, s) - h;
    vec2 i = p/s;
    float sa = step(dot(a, a), dot(b, b));
    float sb = 1. - sa;
    vec2 f = sa*floor(i) + sb*floor(i+0.5);
    vec2 c = f*s + sa*h;
    return vec4(c, sa*a + sb*b);
}

float hexSphIntersect(vec2 ro, vec2 rd){
    float b = dot(ro, rd);
    float c = dot(ro, ro) - RC*RC;
    float delta = b*b - c;
    return sqrt(delta) - b;
}

vec2 hexNormal(vec2 p) {
    float k = step(0.5*RC, abs(p.y));
    return sign(p)*vec2(1.-0.5*k, k*SQRT32);
}

// visualization: https://editor.p5js.org/krafpy/sketches/TQhvnNLGk
float hexIntersect(vec2 ro, vec2 rd){
    float t = hexSphIntersect(ro, rd);
    vec2 n = hexNormal(ro + rd*t);
    return (RI - dot(ro, n))/dot(rd, n);
}

// from: https://iquilezles.org/articles/distfunctions/
float sdBox(vec3 p, vec3 b) {
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

/*float sdBox(vec2 p, vec2 b) {
  vec2 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,q.y),0.0);
}*/

/*float hexDist(vec2 p){
    p = abs(p);
    return max(p.x, 0.5*(p.x + SQRT3*p.y));
}*/


// from: https://iquilezles.org/articles/distfunctions/
float sdHexPrism(vec3 p, vec2 h){
  vec3 k = vec3(-SQRT32, 0.5, 0.57735);
  p = abs(p);
  p.xz = p.zx;
  p.xz -= 2.0*min(dot(k.xy, p.xz), 0.0)*k.xy;
  vec2 d = vec2(
       length(p.xz-vec2(clamp(p.x,-k.z*h.x,k.z*h.x), h.x))*sign(p.z-h.x),
       p.y-h.y );
  //return min(max(d.x,d.y),0.0) + length(max(d,0.0));
  return length(max(d,0.0));
}


float island(vec2 hexc){
    float d = length(hexc);
    float x = atan(hexc.x, hexc.y)*7.33;
    //d += (sin(x)-cos(sin(x))*sin(x))*6.;
    d += 5.*(2.*noise(vec3(x,0.,0.))-1.);
    float h = smoothstep(30., 0., d) + 2.*smoothstep(10., 0., d);
    return h/3.;
}


// from: https://iquilezles.org/articles/smin/
/*float smin(float a, float b, float k) {
    //return -log2(exp2(-k*a)+exp2(-k*b))/k;
    //k *= 2.0;
    float x = b-a;
    return 0.5*( a+b-sqrt(x*x+k*k) );
}*/

float defaultHexHeight = 1000.;


float islandAltitude(){
    float t = (params.z - 62.);
    t *= 0.6;
    float islandTargetAltitude = 20.;
    return islandTargetAltitude*step(0.,t)*smoothstep(0., islandTargetAltitude, t);
}

vec2 getHexHeights(vec2 hexCenter, vec3 rand) {
    float islandScale = island(hexCenter);
    float isIsland = 1.-step(islandScale,0.);
    
    float islandHexHeight = 1.2 + rand.x + 20.*islandScale;
    
    float hexHeight = mix(defaultHexHeight, islandHexHeight, isIsland);
    
    float t = params.z;
    float islandHexDispY;
    // cone-like displacement of individual hexagons
    float islandTargetHexDispY = 15.*islandScale + 1.*rand.y*(1.-islandScale) + 1.;
    // island build start
    t -= 17.;
    float s = step(length(hexCenter), 3.);
    float rh = mix(rand.z*5., 4.5, s);
    islandHexDispY = islandTargetHexDispY*smoothstep(0.,islandTargetHexDispY*0.1,t/6.-rh);
    // altitude of the whole island
    islandHexDispY += islandAltitude();
    
    float hexRandY = -rand.x*0.5*smoothstep(50., 40., length(hexCenter));
    
    float hexDispY = hexRandY + islandHexDispY*isIsland;
    
    return vec2(hexHeight, hexDispY);
}

vec2 mapHexs(vec3 p, vec4 hex){
    vec3 q = vec3(p.x-hex.x, p.y, p.z-hex.y);
    vec2 hexCenter = hex.xy;
    
    vec3 rand = hash32(hexCenter);

    vec2 heights = getHexHeights(hexCenter, rand);
    float hexDispY = heights.y;
    float hexHeight = heights.x;
    
    float hexRad = 0.1;
    vec2 mainHex = vec2(RI-hexRad-0.02, hexHeight);
    float d = sdHexPrism(q-vec3(0.,-hexHeight+hexDispY,0.), mainHex) - hexRad;
    
    /*vec2 groove = vec2(0.7, 0.1);
    float dGroove = sdHexPrism(q-vec3(0.,hexDispY+groove.y,0.), groove) - 0.02;
    float hasGroove = step(rand.y, 0.5);
    d = mix(d, smin(d, dGroove, 0.02), hasGroove);*/
    
    // underground hexagons
    vec2 underHex = vec2(0.7, defaultHexHeight);
    float underOff = -5. - mainHex.y;
    float dUnder = sdHexPrism(q-vec3(0.,-underHex.y + underOff,0.), underHex) - 0.1;
    float isUnder = step(0., d-dUnder);
    d = min(d, dUnder);
    
    
    return vec2(d, isUnder);
}


vec3 cubePos() {
    float t = params.z - 95.;
    float y = mix(-5000., -25. + t*2., step(0.,t));
    
    float a = smoothstep(-15.,-6.,y);
    float b = smoothstep(5.,18.,y);
    float c = smoothstep(20.,45.,y);
    float r = mix(0.,mix(12.5,mix(34.,0.,c),b),a);
    vec3 pos = vec3(r,y,0.);
    pos.xz *= rot(t*0.5);
    return pos;
}

vec3 cubeTransform(vec3 p) {
    p -= cubePos();
    p.xz *= rot(3.*params.z);
    p.yx *= rot(3.7*params.z);
    p.yz *= rot(-1.9*params.z);
    p.zx *= rot(1.2*params.z + sin(3.*params.z));
    return p;
}

float mapCube(vec3 p){ // x = sdf
    p = cubeTransform(p);
    
    vec3 b = vec3(0.5, 0.5, 0.5);
    float d = sdBox(p, b) - 0.4;
    return (d + 0.1/length(p))*0.8;
}

float map(vec3 p) {
    float d = mapHexs(p, hexCell(p.xz)).x;
    if(params.z > 95.){
        d = min(d, mapCube(p));
    }
    return d;
}


vec2 traverse(vec3 ro, vec3 rd, int maxCells, int maxRMSteps, bool calcShad) { // t, shad
    float shad = 1.;
    float t1 = 0.;
    for(int i = 0; i < maxCells; i++){
        vec3 p = ro + rd*t1;
        
        if(!calcShad && length(p.xz) > 55. && ro.y > 0.1){
            float t = (0.1-ro.y)/rd.y;
            return vec2(t, shad);
        }
        
        vec4 hex = hexCell(p.xz);
        float t2max = hexIntersect(hex.zw, normalize(rd.xz))/length(rd.xz);
        
        float t2 = 0.;
        for(int j = 0; j < maxRMSteps-i/2 && t2 < t2max; j++){
            vec3 q = p + rd*t2;
            
            float d = map(q);
            float t = t1 + t2;
            
            // soft shadows
            if(calcShad) {
                vec2 u = vec2(rd.z,-rd.x);
                float d2 = mapHexs(q, hexCell(q.xz + u*RI)).x;
                float d3 = mapHexs(q, hexCell(q.xz - u*RI)).x;

                float dmin = min(d,min(d2,d3));
                shad = min(shad, 32.*dmin/t);
            }
            
            if(d < 0.003) {
                return vec2(t, 0.);
            }
            t2 += d;
        }
        
        t1 += t2max + 0.0001;
    }
    return vec2(-1., shad);
}


vec3 normal(vec3 p){
    vec2 h = 0.0001*vec2(1.,-1.);
    return normalize(
        map(p + h.xyy)*h.xyy +
        map(p + h.yyx)*h.yyx +
        map(p + h.yxy)*h.yxy +
        map(p + h.xxx)*h.xxx
    );
}



vec3 skyCol = vec3(0.8,0.82,0.9);
vec3 sunCol = vec3(1.2, 1.1, 1.);
vec3 sunDir = normalize(vec3(-0.6, 0.4, 1.));

vec3 background(vec3 ro, vec3 rd) {
    float s = max(dot(rd, sunDir), 0.);
    vec3 bg = mix(skyCol, sunCol, pow(s,8.0));
    s = max(-rd.y, 0.);
    bg = mix(bg, vec3(0.), s);
    return bg;
}

vec3 fog(vec3 col, float t, vec3 ro, vec3 rd) {
    float strength = 1. - exp(-0.0001*t*t);
    return mix(col, background(ro, rd), strength);
}

float ao(vec3 p, vec3 n){
    float occ = 0.;
    float s = 2.;
    for(int i = 0; i < 5; i++){
        float t = 0.01 + float(i)*0.13;        
        //float d = map(p + t*n);
        vec3 q = p + t*n;
        vec2 u = vec2(n.z,-n.x);
        float d = mapHexs(q, hexCell(q.xz)).x;
        float d2 = mapHexs(q, hexCell(q.xz + u*RI)).x;
        float d3 = mapHexs(q, hexCell(q.xz - u*RI)).x;
        d = min(d, min(d2, d3));
        occ += s*(t-d);
        s *= 0.6;
    }
    return clamp(1.-occ, 0., 1.);
}

float shadow(vec3 p, vec3 n){
    vec3 ro = p + n*0.011;
    return traverse(ro, sunDir, 45, 25, true).y;
}

/*float saturate(float x) {
    return clamp(x,0.,1.);
}*/

/*float G_GGXp(vec3 v, vec3 n, vec3 h, float a){
    float VdotH = saturate(dot(v,h));
    float chi = step(0., VdotH/saturate(dot(v,n)));
    float VdotH2 = VdotH * VdotH;
    float tan2 = (1. - VdotH2) / VdotH2;
    return (chi * 2.) / (1. + sqrt(1. + a*a*tan2));
}

float G_GGX(vec3 wi, vec3 wo, vec3 n, vec3 h, float a){
    return G_GGXp(wo,n,h,a)*G_GGXp(wi,n,h,a);
}*/

vec3 lighting(vec3 p, vec3 ro, vec3 rd) {
    vec3 col = vec3(0.);

    vec4 hex = hexCell(p.xz);
    vec2 hexc = hex.xy;
    vec3 rand = hash32(hexc);

    float isUnder = mapHexs(p, hex).y;
    
    float cubeDst = mapCube(p);
    vec3 cubeRelPos = cubeTransform(p);
    float isCube = step(cubeDst,0.01);
    float cubeNoise = 1.-pow(fbm(cubeRelPos), 5.);
    float cubeRough = clamp(1.-cubeNoise, 0.1, 1.);

    vec3 q = p;
    q.y -= getHexHeights(hexc, rand).y;

    vec3 n = normal(p);
    
    vec3 l = sunDir;
    vec3 v = -rd;
    vec3 h = normalize(l + v);
    
    // cook-torrance shading for direct sun light
    float NdotL = clamp(dot(n, l), 0.0, 1.0);
    float NdotV = clamp(dot(n, v), 0.0, 1.0);
    float NdotH = clamp(dot(n, h), 0.0, 1.0);
    float LdotH = clamp(dot(l, h), 0.0, 1.0);
    float VdotH = clamp(dot(v, h), 0.0, 1.0);
    
    float rough = mix(mix(0.1,0.7,rand.y) + 0.05*sin(PI*pow(fbm(2.*q),6.)), cubeRough, isCube);
    float rough2 = rough*rough;
    float D = pow(NdotH,2./rough2-2.)/(PI*rough2);
    
    float g = 2.*NdotH/VdotH;
    float G = min(1., g*min(NdotV, NdotH));
    //float G = G_GGX(v, sunDir, n, h, rough);
    
    //vec3 F0 = mix(vec3(0.664,0.824,0.850), vec3(0.549,0.556,0.554), hash12(hexc+vec2(7.045)));
    float rnd = hash32(hexc+vec2(7.045)).x;
    vec3 F0 = mix(vec3(0.764,0.824,0.850), vec3(0.972,0.960,0.915), rnd);
    // if underground hexagon
    vec3 F0u = mix(vec3(0.664,0.824,0.850), vec3(0.549,0.556,0.554), rnd);
    F0 = mix(F0, F0u, isUnder);
    // if cube
    F0 = mix(F0, vec3(0.045), isCube);
    
    vec3 F = F0 + (1.-F0)*pow(1.-LdotH,5.);
    
    vec3 metal = vec3(0.5);
    vec3 diffc = (1.-metal)*F0/metal;
    diffc *= mix(diffc, diffc*vec3(0.7), pow(fbm(30.*q),5.)+0.5*pow(fbm(2.*q),6.));
    
    vec3 diff = diffc*NdotL;
    vec3 spec = (D*G*F)/(4.*max(0.0001, dot(n,v)));
    float s = 0.5;
    col = sunCol*((1.-s)*diff + s*spec);
    
    // shadows (direct lighting only)
    col *= shadow(p, n);
    
    // sky color
    vec3 indirect = 0.1*skyCol;
    // fake global illumination
    indirect += 0.1*sunCol*max(0., dot(n, -vec3(sunDir.x, 0., sunDir.z)));
    indirect += 0.03*skyCol*max(0., dot(n, vec3(sunDir.x, 0., sunDir.z)));
    
    indirect *= smoothstep(-1.-islandAltitude(),0.,p.y);
    
    col += indirect*diffc;
    
    col *= ao(p, n);
    
    // light cube illumination
    vec3 cubeCol = vec3(1.,0.5, 0.3);
    float atten = 3. / (1.+cubeDst*cubeDst);
    col += cubeCol * atten * max(0., dot(normalize(cubePos()-p),n)) * (1.-isCube);
    col += 2.*cubeCol * cubeNoise * isCube * smoothstep(1.6,1.,length(cubePos()-p));
    
    return col;
}

float t0 = 0.;
vec3 at, ro, rd;
float T(float duration, vec3 target, vec3 offset) {
    if(params.z>=t0){
        at = target;
        ro = target + offset;
    }
    t0 += duration;
    return params.z - t0;
}

void main()
{
    vec2 uv = gl_FragCoord.xy/params.xy;
    vec3 prevCol = texture(previousFrame, uv).xyz;
    uv -= 0.5;
    uv.x *= params.x/params.y;
    
    vec3 col = vec3(0.);
    
    // animation and timing
    vec3 dir = vec3(1.,0.,1.);
    vec3 pos = vec3(0.,0.,-10.);
    vec2 rt, ct;
    float t;
    
    // 0
    t = T(5., pos + dir*params.z, -1.*dir + vec3(0.,10.,0.));
    // 5
    t = T(5., pos + dir*params.z, 15.*vec3(dir.z,0.,-dir.x) + vec3(0.,15.,0.));
    // 10
    t = T(5., pos + dir*params.z, 10.*vec3(dir.z,0.,-dir.x) + vec3(0.,1.,0.));
    
    pos = vec3(-11.,0.,-37.);
    dir = vec3(0.2,0.,1.);
    // 15
    t = T(5., pos + dir*params.z*0.6, -5.*dir + vec3(2.,7.,0.));
    
    // 20
    rt = 40.*sincos(0.1*params.z);
    t = T(5., vec3(0.), vec3(rt.x,10.,rt.y));
    // 25
    rt *= 0.2;
    t = T(5., vec3(0.,t+7.,0.), vec3(rt.y,5.,rt.x));
    
    // 30
    pos = vec3(0.,10.,0.) - dir*5.;
    t = T(5., pos + t*dir, 10.*vec3(dir.z,0.,-dir.x));
    
    // 35
    rt = 20.*sincos(0.1*params.z);
    ct = 20.*sincos(0.1*params.z-0.5);
    t = T(5., vec3(rt.x,5.,rt.y), vec3(ct.x,10.,ct.y));
    
    // 40
    rt = 15.*sincos(0.1*params.z+1.);
    ct = 2.*sincos(0.1*params.z+1.-PI/2.);
    t = T(5., vec3(rt.y,5.,rt.x), vec3(ct.y,1.,ct.x));
    
    // 45
    rt = sincos(0.1*params.z-0.8);
    t = T(2.5, vec3(0.,t+6.,0.), vec3(rt.y,5.,rt.x));
    // 47.5
    rt = sincos(0.1*params.z-0.5);
    t = T(2.5, vec3(0.,t+6.,0.), vec3(rt.y,15.,rt.x));
    // 50
    rt = sincos(0.1*params.z-0.3);
    t = T(5., vec3(0.,t+6.,0.), vec3(rt.y,25.,rt.x));
    // 52.5
    // rt = sincos(0.1*params.z-0.0);
    // t = T(2.5, vec3(0.,t+6.,0.), vec3(rt.y,35.,rt.x));
    
    // 55
    rt = 30.*sincos(0.1*params.z);
    t = T(5., vec3(0.,10.,0.), vec3(rt.y,5.,rt.x));
    
    // 60
    ct = 40.*sincos(3.3-t*0.05);
    t = T(5., vec3(0.,0.,0.), vec3(ct.x,1.,ct.y));
    
    // 65
    ct = 40.*sincos(0.03*t);
    t = T(5., vec3(0.,0.,0.), vec3(ct.x,1.,ct.y));

    // 70
    rt = sincos(0.1*params.z-0.3);
    t = T(5., vec3(0.,0.,0.), vec3(rt.y, 50.,rt.x));
    
    // 75
    ct = 50.*sincos(0.02*t+3.);
    t = T(5., vec3(0.,-1.,0.), vec3(ct.y,10.,ct.x));
    
    // 80
    t = T(10., vec3(-30.+t,0.+0.7*t,0.), vec3(-4.,1.-0.5*t,-1.));
    
    // 90
    ct = 20.*sincos(t*0.1+3.);
    t = T(5., vec3(0.,-5.+t*0.5,0.), vec3(ct.x,-t*0.5,ct.y));
    
    // 95
    vec3 c = cubePos();
    T(10., vec3(0.,c.y,0.), vec3(ct.y,-c.y,ct.x));
    
    // 105
    ct *= rot(PI);
    t = T(5., vec3(0.,c.y,0.), vec3(ct.x,0.,ct.y));
    
    // 110
    ct = 10.*sincos(-0.1*t-1.5);
    t = T(5., c, vec3(ct.x,-c.y+2.,ct.y));
    
    // 115
    ct = 50.*sincos(0.5*t+4.);
    t = T(5., vec3(0.,c.y,0.), vec3(ct.y,10.,ct.x));
    
    // 120
    t = T(5., c, -c + vec3(7., 35., 0.));
    
    // 125
    t = T(5., vec3(0.,35.+0.5*t,0.), vec3(-25.,0.,0.));
    
    // 130
    t = T(10., vec3(0.,40.,0.), vec3(t+45.,0.,0.));
    
    
    // rays
    vec3 forward = normalize(at - ro);
    vec3 right = normalize(vec3(forward.z, 0., -forward.x)); // 90° rotation
    vec3 up = cross(forward, right);

    uv += (2.*hash32(333.*uv+params.z).xy-1.)*0.0005; // randomize edges for a "good enough" antialiasing
    rd = normalize(uv.x*right + uv.y*up + forward);
    
    outCol = vec4(1.,0.,0.,1.);
    
    float h = traverse(ro, rd, 100, 100, false).x;
    if(h > 0.) {
        vec3 p = ro + rd*h;
        col = lighting(p, ro, rd);
        col = fog(col, h, ro, rd);
    } else {
        col = background(ro, rd);
    }
    
    col = clamp(col, 0., 1.);
    col = pow(col, vec3(0.45)); // gamma correction
    
    // fade in/out
    col *= smoothstep(0.,1.,params.z);
    // fade out
    col *= smoothstep(136.,135.,params.z);
    
    col = mix(col, prevCol, 0.4); // motion blur
    outCol = vec4(col,1.0);
}