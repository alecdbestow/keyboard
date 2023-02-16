#ifndef _ACTION_STREAM_H_
#define _ACTION_STREAM_H_

#include "meta.hpp"
#include "stroke.hpp"
#include "dictionary.hpp"


#define MAX_STORED_ACTIONS_LENGTH 20
#define MAX_TRANSLATION_LENGTH 100
#define MAX_OUTPUT_LENGTH MAX_STORED_ACTIONS_LENGTH * MAX_TRANSLATION_LENGTH
#define MAX_NUM_STROKES 11

#define POST 1
#define PRE 1<<1


class ActionStream;
class CircAP  {
    public:
        CircAP(size_t l, size_t index = 0);
        CircAP(const CircAP &circap);
        size_t getBounded(int i);
        size_t getLength() const {return len;}
        size_t getPos() const {return pos;}
        CircAP operator+ (const CircAP &circap);
        CircAP operator+ (const int i);
        CircAP operator- (const CircAP &circap);
        CircAP operator- (const int i);
        void operator++ (int);
        void operator+= (const CircAP &circap);
        bool operator== (const CircAP &circap);
        bool operator!= (const CircAP &circap);
        operator size_t() const;
        
    private:
        size_t len;
        size_t pos;
};
class CompilerIndex;

typedef void (*Func)(ActionStream &, char);

typedef struct compileMatch
{
    char match[10];
    Func func;
    char arg;
    
    uint8_t order;
    uint8_t prefixPos;
} CompileMatch;

class CompilerIndex
{
    public:
        CompilerIndex(char *actionsOutput, char* output, CircAP idx);
        bool outputOnce(ActionStream &a, bool checkIndex = true);
        void setIndex(const CircAP &idx)  {index = index;}
        void addToIndex(int i)  {index += i;}
        const CircAP &getIndex() const  {return index;}
        void setActionIndex(char *idx)   {actionIndex = idx;}
        void setOutIndex(char *idx)    {outIndex = idx;}
        void skipPrefix(const char *prefixString);
        char *outIndex;
        char *actionIndex;
        bool inCommand;
        bool capWord;
        bool lowerNext;
        bool capNext;
        bool attachNext;
        bool finishedCapWord;
        char *glue; 
        char spaceString[MAX_TRANSLATION_LENGTH];
    private:
        void outputPrefix();
        void outputEscaped();
        void outputAlpha();
        void outputCommand();
        void outputOther();
        void increment();
        CompileMatch *findMatch(const ActionStream &a) const;
        void callMatchFunc(ActionStream &a, const CompileMatch &match, uint8_t order);
        void performCommandOutput(ActionStream &a, const CompileMatch &match);
        void outputCommand(ActionStream &a);
        
        
        CircAP index;



    
};




class Action{
    public:
        Action();
        const Stroke &getStroke() const   {return stroke;}
        void setStroke(const Stroke &s);
        const size_t getLength() const  {return length;}
        void setLength(size_t len)  {length = len;}
        char *getTranslation() const   {return translation;}
        void setTranslation(char *trans)    {translation = trans;}
    private:
        Stroke stroke;
        char *translation;
        size_t length;
};


class ActionStream {
    public:
        ActionStream();
        void update(const Stroke &stroke);
        CompilerIndex ci; 
        CircAP endPoint;
        Action actions[MAX_STORED_ACTIONS_LENGTH];
        char actionsOutput[MAX_OUTPUT_LENGTH];
        char output[MAX_OUTPUT_LENGTH];
    private:
        void outputAction();
        void updateActionsOutput();
        void compileOutput();
        void getCombinedStrokes(char* strokes, CircAP start);
        void addStroke(const Stroke &stroke);
        size_t addTranslationToOutput(CircAP index, char *pos, bool &found);
        Dictionary dict;
        
    
};







static size_t lstrcpy(char *dest, const char *source);

//void compileActions(ActionStream *a);
void compileOutput(ActionStream *a);
void outputCommand(ActionStream *a);
void callMatchFunc(ActionStream *a, CompileMatch *match, uint8_t order);
void skipPrefix(ActionStream *a, char *match);
CompileMatch *findMatch(ActionStream *a);
bool prefix(const char *pre, const char *str);



#endif