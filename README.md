
# Comp Hw5:
importent:
1. current_register must also be a filed in the type.h Node as sometimed the parser is the only one that can set it. will explain in person.

what is now :
1. I added code to prosses addetive, like int x = 5 + 4; and byte y = a + b; when a and b are known 
2. Added the ID option to assign 

what is a problem:
1. in the isdtuction it says we need to save the local var of a function at its start acurding to its offset.
the only way I think we can do it is by setting 50 at the start. but this is gonna be soo much work! Gal didnt do it and I think we shouldnt too.












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

