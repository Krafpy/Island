// Generated with Shader Minifier 1.3.6 (https://github.com/laurentlb/Shader_Minifier/)
#ifndef SHADERS_INL_
# define SHADERS_INL_
# define VAR_outCol "r"
# define VAR_params "v"
# define VAR_previousFrame "y"

const char *main_frag =
 "#version 460\n"
 "layout(location=0) uniform vec4 v;"
 "layout(binding=0) uniform sampler2D y;"
 "out vec4 r;"
 "vec2 p(float v)"
 "{"
   "return vec2(sin(v),cos(v));"
 "}"
 "mat2 x(float v)"
 "{"
   "vec2 m=p(v);"
   "return mat2(m.y,-m.x,m.x,m.y);"
 "}"
 "float t(vec3 v)"
 "{"
   "v=fract(v*.1031);"
   "v+=dot(v,v.zyx+31.32);"
   "return fract((v.x+v.y)*v.z);"
 "}"
 "vec3 m(vec2 v)"
 "{"
   "vec3 m=fract(vec3(v.xyx)*vec3(.1031,.103,.0973));"
   "m+=dot(m,m.yxz+33.33);"
   "return fract((m.xxy+m.yzz)*m.zyx);"
 "}"
 "float n(vec3 v)"
 "{"
   "vec3 z=floor(v),m=smoothstep(0.,1.,v-z);"
   "return mix(mix(mix(t(z+vec2(1,0).yyy),t(z+vec2(1,0).xyy),m.x),mix(t(z+vec2(1,0).yxy),t(z+vec2(1,0).xxy),m.x),m.y),mix(mix(t(z+vec2(1,0).yyx),t(z+vec2(1,0).xyx),m.x),mix(t(z+vec2(1,0).yxx),t(z+vec2(1,0).xxx),m.x),m.y),m.z);"
 "}"
 "float s(vec3 v)"
 "{"
   "float z=0.,m=.5;"
   "for(int r=0;r<8;r++)"
     "z+=m*n(v),v*=2.,m*=.6;"
   "return z;"
 "}"
 "vec4 f(vec2 v)"
 "{"
   "vec2 m=vec2(1,1.73205080757)*2.,y=m*.5,z=mod(v,m)-y,x=mod(v+y,m)-y,d=v/m;"
   "float f=step(dot(z,z),dot(x,x)),g=1.-f;"
   "return vec4((f*floor(d)+g*floor(d+.5))*m+f*y,f*z+g*x);"
 "}"
 "float f(vec2 v,vec2 z)"
 "{"
   "float m=dot(v,z);"
   "return sqrt(m*m-dot(v,v)+1.33333333333506)-m;"
 "}"
 "vec2 e(vec2 v)"
 "{"
   "float m=step(.57735026919,abs(v.y));"
   "return sign(v)*vec2(1.-.5*m,m*.86602540378);"
 "}"
 "float e(vec2 v,vec2 z)"
 "{"
   "float m=f(v,z);"
   "vec2 y=e(v+z*m);"
   "return(1.-dot(v,y))/dot(z,y);"
 "}"
 "float h(vec3 v)"
 "{"
   "vec3 m=abs(v)-vec3(.5);"
   "return length(max(m,0.))+min(max(m.x,max(m.y,m.z)),0.);"
 "}"
 "float h(vec3 v,vec2 m)"
 "{"
   "v=abs(v);"
   "v.xz=v.zx;"
   "v.xz-=2.*min(dot(vec3(-.86602540378,.5,.57735).xy,v.xz),0.)*vec3(-.86602540378,.5,.57735).xy;"
   "vec2 z=vec2(length(v.xz-vec2(clamp(v.x,-vec3(-.86602540378,.5,.57735).z*m.x,vec3(-.86602540378,.5,.57735).z*m.x),m))*sign(v.z-m.x),v.y-m.y);"
   "return length(max(z,0.));"
 "}"
 "float w(vec2 v)"
 "{"
   "float z=length(v);"
   "z+=5.*(2.*n(vec3(atan(v.x,v.y)*7.33,0,0))-1.);"
   "float m=smoothstep(30.,0.,z)+2.*smoothstep(10.,0.,z);"
   "return m/3.;"
 "}"
 "float e()"
 "{"
   "float z=v.z-62.;"
   "z*=.6;"
   "return 20.*step(0.,z)*smoothstep(0.,20.,z);"
 "}"
 "vec2 m(vec2 z,vec3 m)"
 "{"
   "float y=w(z),x=1.-step(y,0.),f=v.z,r,s=15.*y+m.y*(1.-y)+1.;"
   "f-=17.;"
   "r=s*smoothstep(0.,s*.1,f/6.-mix(m.z*5.,4.5,step(length(z),3.)));"
   "r+=e();"
   "float g=-m.x*.5*smoothstep(50.,40.,length(z))+r*x;"
   "return vec2(mix(1e3,1.2+m.x+20.*y,x),g);"
 "}"
 "vec2 n(vec3 v,vec4 z)"
 "{"
   "vec3 d=vec3(v.x-z.x,v.y,v.z-z.y),y=m(z.xy);"
   "vec2 x=m(z.xy,y),g=vec2(.88,x);"
   "float f=h(d-vec3(0,-x.x+x.y,0),g)-.1,s=h(d-vec3(0,-vec2(.7,1e3).y+(-5.-g.y),0),vec2(.7,1e3))-.1,b=step(0.,f-s);"
   "f=min(f,s);"
   "return vec2(f,b);"
 "}"
 "vec3 f()"
 "{"
   "float z=v.z-95.,y=mix(-5e3,-25.+z*2.,step(0.,z));"
   "vec3 m=vec3(mix(0.,mix(12.5,mix(34.,0.,smoothstep(20.,45.,y)),smoothstep(5.,18.,y)),smoothstep(-15.,-6.,y)),y,0);"
   "m.xz*=x(z*.5);"
   "return m;"
 "}"
 "vec3 d(vec3 z)"
 "{"
   "z-=f();"
   "z.xz*=x(3.*v.z);"
   "z.yx*=x(3.*v.z);"
   "return z;"
 "}"
 "float c(vec3 v)"
 "{"
   "v=d(v);"
   "float z=h(v)-.4;"
   "return(z+.1/length(v))*.8;"
 "}"
 "float a(vec3 z)"
 "{"
   "float m=n(z,f(z.xz)).x;"
   "if(v.z>95.)"
     "m=min(m,c(z));"
   "return m;"
 "}"
 "vec2 a(vec3 v,vec3 m,int y,int z,bool x)"
 "{"
   "float r=1.,s=0.;"
   "for(int i=0;i<y;i++)"
     "{"
       "vec3 d=v+m*s;"
       "if(!x&&length(d.xz)>55.&&v.y>.1)"
         "return vec2((.1-v.y)/m.y,r);"
       "vec4 g=f(d.xz);"
       "float c=e(g.zw,normalize(m.xz))/length(m.xz),p=0.;"
       "for(int l=0;l<z-i/2&&p<c;l++)"
         "{"
           "vec3 b=d+m*p;"
           "float t=a(b),u=s+p;"
           "if(x)"
             "{"
               "vec2 h=vec2(m.z,-m.x);"
               "float w=n(b,f(b.xz+h)).x,C=n(b,f(b.xz-h)).x;"
               "r=min(r,32.*min(t,min(w,C))/u);"
             "}"
           "if(t<.003)"
             "return vec2(u,0);"
           "p+=t;"
         "}"
       "s+=c+1e-4;"
     "}"
   "return vec2(-1,r);"
 "}"
 "vec3 l(vec3 v)"
 "{"
   "vec2 m=1e-4*vec2(1,-1);"
   "return normalize(a(v+m.xyy)*m.xyy+a(v+m.yyx)*m.yyx+a(v+m.yxy)*m.yxy+a(v+m.xxx)*m.xxx);"
 "}"
 "vec3 z=normalize(vec3(-.6,.4,1));"
 "vec3 a(vec3 v,vec3 m)"
 "{"
   "float y=max(dot(m,z),0.);"
   "vec3 x=mix(vec3(.8,.82,.9),vec3(1.2,1.1,1),pow(y,8.));"
   "y=max(-m.y,0.);"
   "return mix(x,vec3(0),y);"
 "}"
 "vec3 a(vec3 v,float z,vec3 m,vec3 y)"
 "{"
   "return mix(v,a(m,y),1.-exp(-1e-4*z*z));"
 "}"
 "float c(vec3 v,vec3 m)"
 "{"
   "float z=0.,r=2.;"
   "for(int y=0;y<5;y++)"
     "{"
       "float d=.01+float(y)*.13;"
       "vec3 x=v+d*m;"
       "vec2 g=vec2(m.z,-m.x);"
       "float b=n(x,f(x.xz)).x,s=n(x,f(x.xz+g)).x,t=n(x,f(x.xz-g)).x;"
       "b=min(b,min(s,t));"
       "z+=r*(d-b);"
       "r*=.6;"
     "}"
   "return clamp(1.-z,0.,1.);"
 "}"
 "vec3 a(vec3 v,vec3 y,vec3 x)"
 "{"
   "vec3 r=vec3(0);"
   "vec4 b=f(v.xz);"
   "vec3 g=m(b.xy);"
   "float t=n(v,b).y,p=c(v);"
   "vec3 i=d(v);"
   "float u=step(p,.01),C=1.-pow(s(i),5.);"
   "vec3 h=v;"
   "h.y-=m(b.xy,g).y;"
   "vec3 w=normalize(l(v)+(1.-u)*.03*sin(acos(-1.)*vec3(pow(s(2.*h),6.)))),D=-x,B=normalize(z+D);"
   "float A=clamp(dot(w,B),0.,1.),E=mix(mix(.1,.7,g.y),clamp(1.-C,.1,1.),u),F=E*E,G=m(b.xy+vec2(7.045)).x;"
   "vec3 o=mix(vec3(.764,.824,.85),vec3(.972,.96,.915),G);"
   "o=mix(o,mix(vec3(.664,.824,.85),vec3(.549,.556,.554),G),t);"
   "o=mix(o,vec3(.045),u);"
   "vec3 H=o+(1.-o)*pow(1.-clamp(dot(z,B),0.,1.),5.),I=(1.-vec3(.5))*o/vec3(.5);"
   "I*=mix(I,I*vec3(.7),pow(s(30.*h),5.)+.5*pow(s(2.*h),6.));"
   "vec3 J=I*clamp(dot(w,z),0.,1.);"
   "r=vec3(1.2,1.1,1)*(.5*J+.5*(pow(A,2./F-2.)/(acos(-1.)*F)*min(1.,2.*A/clamp(dot(D,B),0.,1.)*min(clamp(dot(w,D),0.,1.),A))*H/(4.*max(1e-4,dot(w,D)))));"
   "r*=a(v+w*.011,z,45,25,true).y;"
   "vec3 K=.1*vec3(.8,.82,.9);"
   "K+=.1*vec3(1.2,1.1,1)*max(0.,dot(w,-vec3(z.x,0,z.z)));"
   "K+=.03*vec3(.8,.82,.9)*max(0.,dot(w,vec3(z.x,0,z.z)));"
   "K*=smoothstep(-1.-e(),0.,v.y);"
   "r+=K*I;"
   "r*=c(v,w);"
   "r+=vec3(1,.5,.3)*(3./(1.+p*p))*max(0.,dot(normalize(f()-v),w))*(1.-u);"
   "r+=2.*vec3(1,.5,.3)*C*u*smoothstep(1.6,1.,length(f()-v));"
   "return r;"
 "}"
 "float i=0.;"
 "vec3 u,b,g;"
 "float c(float m,vec3 z,vec3 y)"
 "{"
   "if(v.z>=i)"
     "u=z,b=z+y;"
   "i+=m;"
   "return v.z-i;"
 "}"
 "void main()"
 "{"
   "vec2 z=gl_FragCoord.xy/v.xy;"
   "vec3 s=texture(y,z).xyz;"
   "z-=.5;"
   "z.x*=v.x/v.y;"
   "vec3 d=vec3(0),i=vec3(1,0,1),t=vec3(0,0,-10);"
   "vec2 w,l;"
   "float h;"
   "h=c(5.,t+i*v.z,-1.*i+vec3(0,10,0));"
   "h=c(5.,t+i*v.z,15.*vec3(i.z,0,-i.x)+vec3(0,15,0));"
   "h=c(5.,t+i*v.z,10.*vec3(i.z,0,-i.x)+vec3(0,1,0));"
   "t=vec3(-11,0,-37);"
   "i=vec3(.2,0,1);"
   "h=c(5.,t+i*v.z*.6,-5.*i+vec3(2,7,0));"
   "w=40.*p(.1*v.z);"
   "h=c(5.,vec3(0),vec3(w.x,10,w.y));"
   "w*=.2;"
   "h=c(5.,vec3(0,h+7.,0),vec3(w.y,5,w));"
   "t=vec3(0,10,0)-i*5.;"
   "h=c(5.,t+h*i,10.*vec3(i.z,0,-i.x));"
   "w=20.*p(.1*v.z);"
   "l=20.*p(.1*v.z-.5);"
   "h=c(5.,vec3(w.x,5,w.y),vec3(l.x,10,l.y));"
   "w=15.*p(.1*v.z+1.);"
   "l=2.*p(.1*v.z+1.-acos(-1.)/2.);"
   "h=c(5.,vec3(w.y,5,w),vec3(l.y,1,l));"
   "w=p(.1*v.z-.8);"
   "h=c(10.,vec3(0,h+6.,0),vec3(w.y,5,w));"
   "w=30.*p(.1*v.z);"
   "h=c(5.,vec3(0,10,0),vec3(w.y,5,w));"
   "l=5.*p(3.3-h*.05);"
   "h=c(5.,vec3(4,0,-30),vec3(l.x,5,l.y));"
   "l=40.*p(.03*h);"
   "h=c(10.,vec3(0),vec3(l.x,1,l.y));"
   "l=50.*p(.02*h+3.);"
   "c(10.,vec3(0,-1,0),vec3(l.y,10,l));"
   "l=20.*p(h*.1+1.);"
   "h=c(10.,vec3(0,-5.+h*.5,0),vec3(l.x,-h*.5,l.y));"
   "vec3 o=f();"
   "c(10.,vec3(0,o.y,0),vec3(l.y,-o.y,l));"
   "l*=x(acos(-1.));"
   "h=c(5.,vec3(0,o.y,0),vec3(l.x,0,l.y));"
   "l=10.*p(-.1*h-1.5);"
   "h=c(5.,o,vec3(l.x,-o.y+2.,l.y));"
   "l=50.*p(.5*h+4.);"
   "h=c(5.,vec3(0,o.y,0),vec3(l.y,10,l));"
   "h=c(5.,o,-o+vec3(7,35,0));"
   "h=c(5.,vec3(0,35.+.5*h,0),vec3(-25,0,0));"
   "h=c(10.,vec3(0,40,0),vec3(h+45.,0,0));"
   "vec3 D=normalize(u-b),C=normalize(vec3(D.z,0,-D.x));"
   "z+=(2.*m(333.*z+v.z).xy-1.)*5e-4;"
   "g=normalize(z.x*C+z.y*cross(D,C)+D);"
   "float I=a(b,g,100,100,false).x;"
   "if(I>0.)"
     "{"
       "vec3 n=b+g*I;"
       "d=a(n,b,g);"
       "d=a(d,I,b,g);"
     "}"
   "else"
     " d=a(b,g);"
   "d=clamp(d,0.,1.);"
   "d=pow(d,vec3(.45));"
   "d*=smoothstep(0.,1.,v.z);"
   "d*=smoothstep(136.,135.,v.z);"
   "d=mix(d,s,.4);"
   "r=vec4(d,1);"
 "}";

#endif // SHADERS_INL_
