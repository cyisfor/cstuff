#ifndef CONCATSYM_H
#define CONCATSYM_H

#define CONCATSYM(a,b) CONCATSYM2(a,b)
#define CONCATSYM2(a,b) a ## b

#define STRIFY(a) STRIFY2(a)
#define STRIFY2(a) #a

#endif /* CONCATSYM_H */
