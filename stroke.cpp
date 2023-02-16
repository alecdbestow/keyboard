#include "stroke.hpp"
#include <string.h>

Stroke::Stroke(){
}

Stroke::Stroke(const char s[NUM_STENO_KEYS])  {
    setTranslation(s);
}

void Stroke::setTranslation(const char s[NUM_STENO_KEYS])    {
    strcpy(stroke, s);
}

Stroke::operator char*() const  {
    return (char *)stroke;
}

void operator+=(char * s, const Stroke& stroke)  {
    strcat(s, "/");
    strcat(s, stroke.stroke);
}