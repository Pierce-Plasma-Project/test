/* Strict-descent probe from the exact block solution (V=0, cost 6).
   Tests every single-variable change, then greedy multi-step descent,
   seeking cost < 6. If none exists reachable, the block obstruction is
   locally irreducible by cross amplitudes. Reuses cross_tab.txt loader. */
#include <stdio.h>
#include <stdlib.h>
int NV,NC,P; int *cmono,*tcnt,**tcoef,**tlen,***tvar,*V,mono[8],nm;
int csum(int ci){long s=0;for(int t=0;t<tcnt[ci];t++){long p=tcoef[ci][t]%P;
 for(int k=0;k<tlen[ci][t];k++){p=(p*V[tvar[ci][t][k]])%P;if(!p)break;}s=(s+p)%P;}return s;}
int cost(){int c=0,mv=-1;for(int ci=0;ci<NC;ci++){int s=csum(ci);
 if(cmono[ci]){if(!s)c+=100;else if(mv<0)mv=s;else if(s!=mv)c+=100;}else if(s)c++;}return c;}
int main(int ac,char**av){P=atoi(av[1]);FILE*f=fopen("cross_tab.txt","r");
 fscanf(f,"%d %d",&NV,&NC);cmono=malloc(NC*4);tcnt=malloc(NC*4);
 tcoef=malloc(NC*sizeof(void*));tlen=malloc(NC*sizeof(void*));tvar=malloc(NC*sizeof(void*));
 for(int ci=0;ci<NC;ci++){fscanf(f,"%d %d",&cmono[ci],&tcnt[ci]);
  tcoef[ci]=malloc(tcnt[ci]*4);tlen[ci]=malloc(tcnt[ci]*4);tvar[ci]=malloc(tcnt[ci]*sizeof(void*));
  for(int t=0;t<tcnt[ci];t++){fscanf(f,"%d %d",&tcoef[ci][t],&tlen[ci][t]);
   tvar[ci][t]=malloc(tlen[ci][t]*4);for(int k=0;k<tlen[ci][t];k++)fscanf(f,"%d",&tvar[ci][t][k]);}}
 fclose(f);V=calloc(NV,4);
 int base=cost();printf("block-solution cost = %d\n",base);
 /* exhaustive single-variable scan over all p-1 nonzero values */
 int bestsingle=base;
 for(int i=0;i<NV;i++)for(int val=1;val<P;val++){V[i]=val;int c=cost();
   if(c<bestsingle){bestsingle=c;printf("single move V[%d]=%d -> cost %d\n",i,val,c);}V[i]=0;}
 printf("best single-variable cost from block solution: %d\n",bestsingle);
 /* greedy: repeatedly take best improving single move */
 int c=base,iter=0;
 for(;;){int bi=-1,bv=0,bc=c;
   for(int i=0;i<NV;i++){int old=V[i];for(int val=0;val<P;val++){if(val==old)continue;
     V[i]=val;int cc=cost();if(cc<bc){bc=cc;bi=i;bv=val;}}V[i]=old;}
   if(bi<0)break;V[bi]=bv;c=bc;iter++;
   if(c<base)printf("greedy step %d: cost %d\n",iter,c);if(c==0)break;}
 printf(c<base?"DESCENT below 6 found: cost %d\n":"NO descent below 6 (local min at block solution): cost %d\n",c);
 return c<base?0:2;}
