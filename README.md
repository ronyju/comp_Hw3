
# Comp Hw5:

***test we need to debug:***
bool foo(){
        return true;
}

void main() {
        while (foo()){
                print("foo is true");
                break;
        }
}

**whats left:**
1. IF
2. While that has open statment.
3. ID ASSIGN Exp SC
4. LPAREN Type RPAREN Exp
5. Override









# comp_Hw3
**Changes 31.5.2023:** 
Download all the files from here as the stuff changed output files.

this are the things I added and tested:
from the instructions : I did 
5 + 8 + 9 + 10 + 16 + 17 (also in the whatsapp group).


**other than that:**
I made sure the override function calling and creating workes and throu the right error.
It required a few changes you can see them in the github diff but I dont think you need to get into it really.

I checked there is a main and that its not override.

I checked for errorAmbiguousCall, and tested it acurding to the yellow lines the stuff added.

**What's left:**
1. handaling the if, else, while scopes.
notice that they wrote something a little wired about how many scopes an if gets, 
I wasnt sure whats the best way to approch it there for I havnt started working in this erea. 
you will also need to check that the break and conitue are inside While scope -> you may use this function that I wrote (return yes if the top scope is WHILE_SCOPE):
bool IsCurrentScopeWhile();

2. you will need to check that byte is not bigger than 255.
thay havnt yet answered me in the piatzza

3. check what number should be printed after every function , should it be 0 for all ?

and of curse debugging the tests... the worst part :(





    
class in data bases:

<img width="521" alt="image" src="https://github.com/ronyju/comp_Hw3/assets/80697658/ab705903-9175-47f4-8b7c-e04eea8c8d71">

