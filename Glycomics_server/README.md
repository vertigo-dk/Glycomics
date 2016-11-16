# adjGrabber


This code creates grabs the lattest news from the Guardian and phrases the text using spacy-nlp in order to extract all adjectives in an article. To monitor the process a simple 

## Installation

Install <a href="https://nodejs.org/en/">Node.js</a>

From the command line:

1. Run <code>npm install</code>

Additionaly the Apple Automator script which can be adjusted to run the server at startup.



## Running

1. Point the terminal to the folder using <code>cd 'path to the folder'</code>
2. start the Node.js server: <code>node adjGrabber.js</code>
3. open a browser and enter <code>localhost:8080/index.html</code> in the adressbar. (when opening from another machine in the network instead of <code>localhost</code> enter the IP of the server.

## Setting up Millumin (OSC receiver)

- <b>Send OSC Messages</b> 
	- to machine <code>localhost</code> 
	- to port <code>7001</code> 
- <b>Listen OSC Messages </b> 
	- from port <code>7002</code>