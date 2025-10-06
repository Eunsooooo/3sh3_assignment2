**3SH3 Assignment 1**
- test commit

Group Members: Eunsu Kim, Momina Hassan

This program works like a very basic shell. It lets the user type commands and runs them using fork() and execvp(). It can also run commands in the background with &, and it keeps the last five commands in a small history. The user can check past commands with history or repeat the most recent one by typing !!.

**Contributions**

Eunsu Kim:
- Implemented command parsing, background execution using &, and the history feature that stores up to five recent commands.
- Added the history and !! commands to view and rerun previous inputs.
- A brief explanation of the code is also included in this README file.


Momina Hassan: 
- Adding code to the main function to create the child process and executing the command in the child 
- Focused on handling the two cases (parent waits while child process executes + parent executes in the background)
- Added commenting
