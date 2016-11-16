var program = require('commander');

// const spacyNLP = require('spacy-nlp')
// // default port 6466
// // start the server with the python client that exposes spacyIO (or use an existing socketIO server at IOPORT)
// var serverPromise = spacyNLP.server({ port: process.env.IOPORT })
// const nlp = spacyNLP.nlp

var WordPOS = require('wordpos'),
    wordpos = new WordPOS();

// Get data from The Guardian
var guardian = require('guardian-news');
guardian.config({
    apiKey: 'b4090503-4d27-4c0f-bf3e-e20b710a58fe'
});

var sentiment = require('multi-sentiment');

// For webserver
var express = require('express');
var app = express();
var server = require('http').createServer(app);
var io = require('socket.io')(server);

// OSC
var osc = require('node-osc');
var client = new osc.Client('127.0.0.1', 3335);
//client.send('/oscAddress', 200, function () { client.kill(); });

var oscServer = new osc.Server(3334, '127.0.0.1');

oscServer.on("/newContent", function (msg, rinfo) {
    console.log("--> received osc message:" + msg);

    var answer =  new osc.Message('/newTree');
    console.log("--> send new content:");
    console.log( JSON.stringify(createTree(), 0, 2));
    answer.append( JSON.stringify(createTree(), 0, 0));
    client.send(answer)

});


function Article(title, text) {
    this.title = title;
    this.text = text;
    this.adjectives;
    this.sentiment;
    this.tree;
};

var articles = [];
// load tree structures
var tree_1 = require('./tree_struktures/tree_1.json');
var tree_2 = tree_1;
var tree_3 = tree_1;

var readAndAnalysed = false;

// program
//     .version('0.0.1')
//     .option('-m --monitor', 'Live monitor the incomming articles')


// if (program.monitor) {
//     setTimeout(function() {

//}, 5000);

server.listen(8080);
console.log('Local server established at port 8080');

// create fileserver
app.use(express.static(__dirname));
app.get('/', function(req, res, next) {
    res.sendFile(__dirname + '/Glycomics-client/app/index.html');
});




grabNewArticles();
setTimeout(function() {
    showOnConsole();
}, 10000);

//update routine every 10min
setInterval(function() {
    var numArticles = articles.length;
    grabNewArticles();
    setTimeout(function() {
        articles.splice(0, numArticles);
        showOnConsole();
    }, 10000);
}, 600000);


//<script src="/socket.io/socket.io.js"></script>
// <script>
// var currentLocation = window.location;
// var socket = io.connect('http://' + currentLocation.hostname + ':8080');
// socket.on('connect', function() {
//     socket.emit('updateContent', {});
// });
//
// socket.on('article', function(article) {
//     if (article.title.length > 0) {
//         document.getElementById("title").innerHTML = article.title;
//     } else {
//         document.getElementById("title").innerHTML = "<p> no server found  </p>";
//     };
// });
// </script>

io.sockets.on('connection', function(socket) {
    console.log('--> page loaded');
    socket.on("updateContent", function(obj) {
        var rndArticle = Math.ceil(Math.random() * articles.length);
        console.log('--> load article ' + rndArticle)
        socket.emit("article", articles[rndArticle] || undefined);
    });
});




function grabNewArticles() {
    guardian.content({
        section: 'world',
        'show-blocks': 'all'
    }).then(function(response) {
        console.log("--> gather articles from the quardian");
        articles = [];
        var results = response.response.results;
        for (i = 0; i < results.length; i++) {
            articles.push(new Article(results[i].webTitle, results[i].blocks.body[0].bodyTextSummary));
        };

        extractAdjectivesWordpos();
        getSentiment();

        readAndAnalysed = true;

    }, function(err) {
        console.log(err);
    });
};


function extractAdjectivesWordpos() {
    console.log("--> extract adjectives from the " + articles.length + " articles");
    for (i = 0; i < articles.length; i++) {
        (function(i) {
            // extract adjectives from the text
            wordpos.getAdjectives(articles[i].text, function(results) {
                var adjectives = results;
                // make all lower case
                for (j = 0; j < adjectives.length; j++) {
                    adjectives[j] = adjectives[j].toLowerCase();
                }
                // sort them to get the longest in an ascending order
                adjectives.sort(function(a, b) {
                    return b.length - a.length
                });

                // for (n = adjectives.length; n > 0; n--) {
                //     console.log(adjectives[n].length);
                //    if (adjectives[n].length < 5) {
                //
                //         adjectives.split(j, adjectives.length-j-1);
                //
                //        n=0;
                //
                //    };
                // };
                // save them into our variable
                articles[i].adjectives = adjectives;
            });
        })(i);
    };
}

// function extractAdjectivesSpacyNLP(){
//     console.log("--> extract adjectives from the "+articles.length+" articles");
//     for (i = 0; i<articles.length; i++){
//         (function(i) {
//             nlp.parse(articles[i].text)
//               .then((output) => {
//                   console.log(output)
//                   console.log(JSON.stringify(output[0].parse_tree, null, 2))
//                   var words = output[0].parse_list;
//                   for(j = 0; j<words.length; j++ ){
//                     //	JJ	Adjective
// 	                //  JJR	Adjective, comparative
//                     //	JJS	Adjective, superlative
//                       if(words[j].POS_fine=="JJ" || words[j].POS_fine=="JJR" || words[j].POS_fine=="JJS"){
//                           //console.log(words[j].word);
//                           var adjective = words[j].word;
//                           articles[i].adjectives.push(adjective);
//                       }
//                   }
//
//                 //console.log(output)
//                 //console.log(JSON.stringify(output[0].parse_tree, null, 2))
//               })
//          })(i);
//     };
// }

function getSentiment() {
    console.log("--> get sentiment from the " + articles.length + " article-titles");
    for (i = 0; i < articles.length; i++) {
        (function(i) {
            // get sentiment from the title
            var string = articles[i].text
            sentiment(string, function(parsed) {
                if (typeof string !== 'string' || string.trim().length == 0) {
                    return;
                }
                articles[i].sentiment = parsed.sentiment.polarity;
                if(articles[i].sentiment=="pos") articles[i].tree = tree_1.branches;
                if(articles[i].sentiment=="neu") articles[i].tree = tree_1.branches;
                if(articles[i].sentiment=="neg") articles[i].tree = tree_1.branches;
            });
        })(i);
    };

}

var createTree = function(){
    var index = Math.ceil(Math.random() * articles.length);
    console.log("--> create tree from article " +index);

    var tree = {};
    tree.title = articles[index].title;
    tree.branches = articles[index].tree;
    var numBranches = Object.keys(tree.branches).length;

    for(i=0; i<numBranches; i++){
        tree.branches[i].adjective = articles[index].adjectives[i];
    }

    var length = articles[index].adjectives.length - numBranches;

    tree.pool = articles[index].adjectives.splice[numBranches, length-1];

    //console.log(tree);

    return tree;
}


function showOnConsole() {
    console.log('');
    console.log('ARTICLES');
    console.log('------');
    for (i = 0; i < articles.length; i++) {
        console.log('TITLE: ' + articles[i].title);
        //console.log('TEXT : ' + articles[i].text);
        console.log('ADJECTIVES #' + articles[i].adjectives.length + ': ' + articles[i].adjectives);
        console.log('SENTIMENT : ' + JSON.stringify(articles[i].sentiment, 0, 4));

        console.log('------');
    }

}