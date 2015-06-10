/**
 * Called whenever serial data is received. Takes in the entire input command and calls 
 * parse() to separate the command and parameters before passing this to execute.
 */
void serialEvent() {
	while (Serial.available()) {
		char inChar = (char) Serial.read();
		//if the end of the string has been reached, parse it and clear our input string
		if (inChar == '/n') {
  			parse(serialInput);
  			serialInput = "";
		}
		//if we're still receiving real letters, add it to our input string
    	else{
      		serialInput += String(inChar);
    	}
  	}
}

/**
 * Parses the input, separating the command and parameter and passing this to execute.
 * 
 * TODO: maybe change this to be recursive, getting the command and multiple parameters 
 * that follow it.
 */
void parse(String input){
	int delimPos = input.indexOf('(');
	String command = "";
	String parameter = "";
	//If the input doesn't have a parameter, only save the command.
	if (delimPos == -1){
		command = input;
	}
	//else, separate the command and parameter from each other
	else{
		command = input.substring(0, delimPos);
		parameter = input.substring(delimPos + 1);
	}

	//Pass the delimited input through to be executed.
	execute(command, parameter);
}
