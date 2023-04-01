#include "formatter.h"

void FormatterInit(Formatter *f)    {
    f->atom = compileMatchingRegex("(\\\\\\{|\\\\\\}|[^\\{\\}])+|\\{(\\\\\\{|\\\\\\}|[^\\{\\}])*\\}");
    f->word = compileMatchingRegex("(\\d+([.,]\\d+)+|[\'\\w]+[-\\w\']*|[^\\w\\s]+)\\s*");
}

void FormatterDo(Formatter *f, Translation *translations)
{
    int i = 0;
    
    int length, exitNum;
    // split all the translations into a long list of actions
    for (int j = 0; j < MAX_STORED_TRANSLATIONS_LENGTH; j++) {
        char *start = translations[j].english;
        while (startsWithRegexN(f->atom, start, &length, &exitNum)) {
            strncpy(f->actions[i].text, start, length);
            start += length;
            i++;
        }
    }
    // format all of the actions in the list
    ActionHandler ah;
    ActionHandlerInit(&ah, f->actions);
    for (int j = 0; j < MAX_NUM_ACTIONS; j++)   {
        ActionFormat(f->actions + i, ActionHandlerGet(ah, f->actions + i));
    }

}

void ActionFormatCommand(Action *a, ActionHandler *ah)
{

}

void ActionFormatWords(Action *a)
{
}

void ActionFormat(Action *a, ActionHandler *ah)
{
    if (a->text[0] == '{')  {
        ActionFormatCommand(a, ah);
    }   else    {
        ActionFormatWords(a);
    }
}

ActionHandler *ActionHandlerGet(ActionHandler *ah, Action *a)
{
    ah->action = a;
    return ah;
}

Action *ActionHandlerNextAction(ActionHandler *ah)
{
    if 
    return nullptr;
}

Action *ActionHandlerPrevAction(ActionHandler *ah)
{
    return nullptr;
}
