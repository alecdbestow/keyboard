#include "action_stream.hpp"

size_t CircAP::getBounded(int i) {

    while (i < 0) {
        i += len;
    }
    if (i >= len) {
        return (size_t)(i% len);
    }   else    {
        return (size_t)i;
    }
}

CircAP::CircAP(const CircAP &circap)    {
    len = circap.getLength();
    pos = circap.getPos();
}


CircAP::CircAP(size_t l, size_t index) : len(l){
    pos = getBounded(index);
}

CircAP CircAP::operator+ (const CircAP &circap) {
    return CircAP(len, circap.getPos() + pos);
}

CircAP CircAP::operator+ (const int i)  {
    return CircAP(len, pos + i);
}

CircAP CircAP::operator- (const CircAP &circap) {
    return CircAP(len, circap.getPos() - pos);
}

CircAP CircAP::operator- (const int i)  {
    return CircAP(len, pos - i);
}

void CircAP::operator++ (int)   {
    pos += 1;
}

void CircAP::operator+= (const CircAP &circap)  {
    
}

bool CircAP::operator== (const CircAP &circap)  {
    return (len == circap.getLength() && pos == circap.getPos());
}

bool CircAP::operator!= (const CircAP &circap)  {
    return (len != circap.getLength() || pos != circap.getPos());
}

CircAP::operator size_t() const {
    return len;
}