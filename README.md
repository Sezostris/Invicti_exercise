Hello!

## General information
The program accomplishes the task of reaching out to "http://testphp.vulnweb.com" and probing its "login.php" endpoint for possible ways to login.

Specifically, the program parses the HTML obtained by performing a GET request on "http://testphp.vulnweb.com/login.php". 

The information yielded by parsing is then "probed" for possible ways of logging in, such as looking for a "loginform" HTML form.

As it turns out, the HTML really does contain such a form, with an endpoint "userinfo.php" along with a description of input fields: "uname" and "pass".

Based on this information, along with a "login and password" (test, test) obtained by eyeballing "http://testphp.vulnweb.com/login.php", the program performs a POST request to "http://testphp.vulnweb.com/userinfo.php" with uname=test and pass=test.
 
This data is encoded in the url itself. 

In the end, we obtain a 200 status which means that we've successfully logged in. Part of the data returned is a session cookie. 

However, cookies are beyond the scope of this project. 

## External requirements
In this project, I've made use of the following external libraries:

`HTTP Requests`: https://github.com/yhirose/cpp-httplib

Added as a git submodule.

`Parsing HTML`: https://github.com/lexborisov/myhtml 

Added as a git submodule.


## Installation:
- In CLI:
```bash
git submodule init
git submodule update
```

- Follow installation steps for myhtml.

- Then:
```bash
cmake .
make
```

- Launch the application
```bash
./main
```

