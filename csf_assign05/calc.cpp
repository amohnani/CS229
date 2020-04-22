//Abhi Mohnani
//amohnan1
#include "calc.h"
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using std::map;
using std::string;
using std::vector;
using std::stringstream;
using std::pair;

struct Calc {
};

class CalcImpl : public Calc {
private:
  map<string, int> vars;

  vector<string> tokenize(const std::string &expr) {
    vector<string> vec;
    stringstream s(expr);

    string tok;
    while (s >> tok) {
        vec.push_back(tok);
    }

    return vec;
  }

  int tokenType(string token) {
    double num;
    stringstream s(token);
    if (token.compare("=") == 0) {
      return 1;
    } else if (token.compare("+") == 0 || token.compare("-") == 0
	       || token.compare("*") == 0 || token.compare("/") == 0) {
      return 2;
    } else if (s >> num) {
      return 3;
    } else {
      return 4;
    }
  }

  //return 0 if failed
  int getVar(string token, int *val) {
    if (vars.find(token) == vars.end()) {
      return 0;
    } else {
      *val = vars.at(token);
      return 1;
    }
  }

  int setVar(string token, int val) {
    vars.insert(pair<string, int>(token, val));
    return 1;
  }

  int singleInput(string token, int *result) {
    int type = tokenType(token);
    if (type == 3) {
      *result = stod(token, NULL);
      return 1;
    }
    if (type == 4) {
      return getVar(token, result);
    }
    return 0;
  }

  int threeInput(vector<string> tokens, int *result) {
    int t1 = tokenType(tokens[0]);
    int t2 = tokenType(tokens[1]);
    int t3 = tokenType(tokens[2]);
    if (t1 == 4 && t2 == 1) { //var =
      if (vars.find(tokens[0]) != vars.end()) {
	vars.erase(tokens[0]);
      }
      if (t3 == 3) { //var = literal
	int num = stod(tokens[2]);
	setVar(tokens[0], num);
	*result = num;
	return 1;
      } else if (t3 == 4) { //var = var
	if (getVar(tokens[2], result) != 0) {
	  return setVar(tokens[0], *result);
	}
      }
    }

    if (t2 == 2) {
      if (t1 == 3 && t3 == 3) {
	return performOperation(stod(tokens[0]), stod(tokens[2]), tokens[1], result);
	
      } else if (t1 == 3 && t3 == 4) {
	int *val = (int *) malloc(4);
	if (getVar(tokens[2], val) != 0) {
	  int a = *val;
	  free(val);
	  return performOperation(stod(tokens[0]), a, tokens[1], result);
	  
	}
      } else if (t1 == 4 && t3 == 3) {
	int *val = (int *)malloc(4);
	if (getVar(tokens[0], val) != 0) {
	  int a = *val;
	  free(val);
	  return performOperation(a, stod(tokens[2]), tokens[1], result);
	  
	}
      } else if (t1 == 4 && t3 == 4) {
	int *val1 = (int *)malloc(4); int *val2 = (int *)malloc(4);
	if (getVar(tokens[0], val1) != 0 && getVar(tokens[2], val2) != 0) {
	  int a = *val1; int b = *val2;
	  free(val1); free(val2);
	  return performOperation(a, b, tokens[1], result);
	  
	}
      }
    }
    
    return 0;
  }

  int fiveInput(vector<string> tokens, int *result) {
    if (tokenType(tokens[1]) == 1 && tokenType(tokens[3]) == 2
	&& tokenType(tokens[0]) == 4) {
      vector<string> expr (tokens.begin() + 2, tokens.end());
      if (threeInput(expr, result) != 0) {
	vector<string> assign (tokens.begin(), tokens.begin() + 2);
	assign.push_back( std::to_string(*result));
	return threeInput(assign, result);
      }
    }
    return 0;
  }

  int performOperation(int val1, int val2, string oper, int *result) {
   
    if (oper.compare("+") == 0) {
      *result = val1 + val2;
      
    } else if (oper.compare("-") == 0) {
      *result = val1 - val2;
    } else if (oper.compare("*") == 0) {
      *result = val1 * val2;
    } else if (oper.compare("/") == 0) {
      if (val2 == 0) {
	return 0;
      }
      *result = val1 / val2;
    }
    return 1;
  }

public:
  int evalExpr(const char *expr, int *result) {
    vector<string> tokens = tokenize(expr);
    if (tokens.size() == 1) {
      return singleInput(tokens[0], result);
    } else if (tokens.size() == 3) {
      return threeInput(tokens, result);
    } else if (tokens.size() == 5) {
      return fiveInput(tokens, result);
    }
    return 0;
  }
};

extern "C" struct Calc *calc_create(void) {
    return new CalcImpl();
}

extern "C" void calc_destroy(struct Calc *calc) {
    CalcImpl *obj = static_cast<CalcImpl *>(calc);
    delete obj;
}

extern "C" int calc_eval(struct Calc *calc, const char *expr, int *result) {
    CalcImpl *obj = static_cast<CalcImpl *>(calc);
    return obj->evalExpr(expr, result);
}
