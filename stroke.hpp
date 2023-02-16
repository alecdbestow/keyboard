#ifndef _STROKE_H_
#define _STROKE_H_

#define NUM_STENO_KEYS 23

class Stroke    {
    public:
        Stroke();
        Stroke(const char s[NUM_STENO_KEYS]);
        void setTranslation(const char s[NUM_STENO_KEYS]);
        const char *getTranslation() const  {return stroke;}

        operator char*() const;
        friend void operator+=(char * s, const Stroke& stroke);
    private:
        char stroke[NUM_STENO_KEYS];
};

//void operator+=(char *, const Stroke&);



#endif