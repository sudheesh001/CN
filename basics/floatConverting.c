#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#define pack754_32(f) (pack754((f),32,8))
#define pack754_64(f) (pack754((f),64,11))
#define unpack754_32(i) (unpack754((i),32,8))
#define unpack754_64(i) (unpack754((i),64,11))
typedef float float32_t;
typedef double float64_t;
uint64_t pack754(long double f,unsigned bits ,unsigned expbits)
{
    long double fnorm;
    int shift;
    long long sign,exp,significand;
    unsigned significandbits = bits - expbits - 1;

    if (f == 0.0) return ;
    if ( f < 0) { sign =1 ; fnorm = -f;}
    else { sign = 0;fnorm = f;}

    shift = 0;
    while (fnorm >= 2.0)
    {
        fnorm /= 2.0;
        shift++;
    }
    while(fnorm < 1.0)
    {
        fnorm *=2.0;
        shift--;
    }
    fnorm-=1.0;

    significand = fnorm * ((1LL<<significandbits) +0.5f);
    exp = shift + ((1<<(expbits - 1)) -1);

    return (sign<<(bits -1 )) | (exp<<(bits - expbits -1)) | significand;
}


long double unpack754(uint64_t i,unsigned bits,unsigned expbits)
{
    long double result;
    long long shift;
    unsigned bias;
    unsigned significandbits = bits - expbits-1;

    if ( i== 0) return 0.0;

    result = (i&((1LL<<significandbits)-1));
    result /= (1LL<<significandbits);
    result +=1.0f;

    bias = (1<<(expbits -1))-1;
    shift = ((i>>significandbits)&((1LL<<expbits)-1))-bias;

    while(shift >0) { result *=2.0;shift--;}
    while(shift <0) { result /= 2.0;shift++;}

    result *=(i>>(bits-1))&1? -1.0:1.0;
    return result;
}



uint32_t htonf(float f)
{
    uint32_t p;
    uint32_t sign;
    if ( f < 0) { sign = 1; f = -f;}
    else { sign = 0;}
    p = ((((uint32_t)f)&0x7fff)<<16) | (sign <<31);
    p |= (uint32_t)(((f-(int)f) * 65536.0f))&0xffff;

    return p;
}
float ntohf(uint32_t p)
{
    float f = ((p>>16)&0x7fff);
    f += (p&0xffff) / 65536.0f;

    if (((p>>31)&0x1) == 0x1) { 
        f =-f;
    }

    return f;
}
void packi16(unsigned char* buf,unsigned int i)
{
    *buf++ = i>>8;
    *buf++ = i;
}

void packi32(unsigned char* buff,unsigned long i)
{
    *buff++ = i>>24;
    *buff++ = i>>16;
    *buff++ = i>>8;
    *buff++ = i;
}

unsigned int unpacki16(unsigned char* buf)
{
    return (buf[0]<<8) | buf[1];
}

unsigned long unpacki32(unsigned char *buf)
{
    return (buf[0]<<24) | (buf[1]<<16) | (buf[2]<<8) | buf[3];

}

int32_t pack(unsigned char *buf,char *format,...)
{
    va_list ap;
    int16_t h;
    int32_t l;
    int8_t c;
    float32_t f;
    char *s;
    int32_t size = 0,len;
    va_start(ap,format);

    for(;*format != '\0'; format++)
    {
        switch(*format){
            case 'h':
                size +=2;
                h = (int16_t)va_arg(ap,int16_t*);
                packi16(buf,h);
                buf+=2;
                break;
             case 'l':
                size+=4;
                l = va_arg(ap,int32_t);
                packi32(buf,l);
                buf +=4;
                break;

             case 'c':
                size+=1;
                c = (int8_t)va_arg(ap,int);
                *buf++ =(c>>0)&0xff;
                break;

             case 'f':
                size +=4;
                f = (float32_t)va_arg(ap,double);
                l = pack754_32(f);
                packi32(buf,l);
                buf +=4;
                break;
             case 's':
                s = va_arg(ap,char*);
                len = strlen(s);
                size += len+2;
                packi16(buf,len);
                buf +=2;
                memcpy(buf,s,len);
                buf += len;
                break;
        }
    }
    va_end(ap);
    return size;
}

void unpack(unsigned char *buf,char *format,...)
{
    va_list ap;
    int16_t *h;
    int32_t *l;
    int8_t *c;
    int32_t pf;
    float32_t *f;
    char* s;
    int32_t len,count,maxstrlen =0;

    va_start(ap,format);

    for(;*format != '\0';format++)
    {
        switch(*format){
            case 'h':
                h = va_arg(ap,int16_t*);
                *h = unpacki16(buf);
                buf+=2;
                break;
            case 'l':
                l = va_arg(ap,int32_t*);
                *l = unpacki32(buf);
                buf +=4;
                break;
            case 'c':
                c = va_arg(ap,int8_t*);
                *c = *buf++;
                break;
            case 's':
                s = va_arg(ap,char*);
                len = unpacki16(buf);
                buf+=2;
                if (maxstrlen > 0 && len >maxstrlen) count = maxstrlen -1;
                else count = len ;
                memcpy(s,buf,count);
                s[count] ='\0';
                buf+=len;
                break;
            default :
                if (isdigit(*format))
                {
                    maxstrlen = maxstrlen * 10 + (*format-'0');
                }
        }
        if (!isdigit(*format)) maxstrlen =0;
    }
    va_end(ap);
}
int main(void)
{
    float f = 3.1415926,f2;
    uint32_t  netf;
    netf = htonf(f);
    f2 = ntohf(netf);

    printf("Original : %f\n",f);
    printf("Network : 0x%08x\n",netf);
    printf("Unpacked: %f\n",f2);


    double d = 3.14159265358979323,d2;
    uint32_t fi;
    uint64_t di;

    fi = pack754_32(f);
    f2 = unpack754_32(fi);

    di = pack754_64(d);
    d2 = unpack754_64(di);
     printf("float before : %.7f\n", f);
     printf("float encoded: 0x%08" PRIx32 "\n", fi);
     printf("float after : %.7f\n\n", f2);
     printf("double before : %.20lf\n", d);
     printf("double encoded: 0x%016" PRIx64 "\n", di);
     printf("double after : %.20lf\n", d2);
 unsigned char buf[1024];
 int8_t magic;
 int16_t monkeycount;
 int32_t altitude;
 float32_t absurdityfactor;
 char *s = "Great unmitigated Zot! You've found the Runestaff!";
 char s2[96];
 int16_t packetsize, ps2;
 packetsize = pack(buf, "chhlsf", (int8_t)'B', (int16_t)0, (int16_t)37,
         (int32_t)-5, s, (float32_t)-3490.6677);
 packi16(buf+1, packetsize); // store packet size in packet for kicks
 printf("packet is %" PRId32 " bytes\n", packetsize);
 unpack(buf, "chhl96sf", &magic, &ps2, &monkeycount, &altitude, s2,
         &absurdityfactor);
 printf("'%c' %" PRId32" %" PRId16 " %" PRId32
         " \"%s\" %f\n", magic, ps2, monkeycount,
         altitude, s2, absurdityfactor);
}
