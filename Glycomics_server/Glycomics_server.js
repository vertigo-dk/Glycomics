
var WordPOS = require('wordpos'),
    wordpos = new WordPOS();

// Get data from The Guardian
var guardian = require('guardian-news');
guardian.config({
    apiKey: 'b4090503-4d27-4c0f-bf3e-e20b710a58fe'
});

var sentiment = require('multi-sentiment');


// OSC
var osc = require('node-osc');
var client = new osc.Client('127.0.0.1', 3335);
var clientWatchdog = new osc.Client('127.0.0.1', 7002);
//client.send('/oscAddress', 200, function () { client.kill(); });

var oscServer = new osc.Server(3334, '127.0.0.1');

var timeToUpdateTree = 30000; // 30sec



function Article(title, text) {
    this.title = title;
    this.text = text;
    this.adjectives;
    this.sentiment;
    this.tree;
};

var articles = [];
// load tree structures
var tree_1 = require('./tree_struktures/tree_2.json');
var tree_2 = require('./tree_struktures/tree_1.json');
var tree_3 = require('./tree_struktures/tree_3.json');

var isGrabbing = false;
var indexArticles = 0;


isGrabbing = true;
grabNewArticles();
setTimeout(function() {
    showOnConsole();
    evaluateArticles();
    showOnConsole();
    isGrabbing = false;
}, 15000);

var numberOfTrees = 0;
var indexTree = 1;
//update routine every 10min
setInterval(function() {
    // send watchdog dead 0
    var m =  new osc.Message('/dead/glycomicsServer');
    m.append(0);
    clientWatchdog.send(m)


    // update tree
    indexTree++;
    if(indexTree == 4) indexTree = 1;
    sendTree(indexTree);

    // each 9 trees, the articles are loaded new
    numberOfTrees++;

    if (numberOfTrees > 9){
        numberOfTrees = 0;
        isGrabbing = true;
        grabNewArticles();
        setTimeout(function() {
            var articlesToRemove = articles.length-10;
            if(articlesToRemove > 0) articles.splice(0, articlesToRemove);
            showOnConsole();
            isGrabbing = false;
        }, 15000);
    }

}, timeToUpdateTree/3);


// ----- OSC RESPONSE -----

oscServer.on("/newContent", function (msg, rinfo) {
    console.log("--> received osc message:" + msg);
    if(isGrabbing){
        setTimeout(sendTree(msg[1]), 15000);
    }else{
        sendTree(msg[1]);
    };
});


function sendTree(treeNr){
    var answer =  new osc.Message('/newTree');
    // console.log("--> send new content:");
    // console.log( JSON.stringify(createTree(), 0, 2));
    answer.append(treeNr);
    answer.append( JSON.stringify(createTree(), 0, 0));
    client.send(answer)
};


// ----- FUNCTIONS -----
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
        //setTimeout(evaluateArticles(), 5000);

    }, function(err) {
        console.log("no internet connection")
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

                //Save only adjectives that are nore than 4 characters long
                tmpAdjectives = []
                adjectives.forEach(function(adj) {
                    if (adj.length >= 4) {
                        tmpAdjectives.push(adj)
                    }
                })

                articles[i].adjectives = tmpAdjectives;
            });
        })(i);
    };
}

function getSentiment() {
    console.log("--> get sentiment from the " + articles.length + " article-titles");
    var iToDelete = [];
    for (i = 0; i < articles.length; i++) {
        (function(i) {
            // get sentiment from the title
            var string = articles[i].text
            sentiment(string, function(parsed) {
                if (typeof string !== 'string' || string.trim().length == 0) {
                    return;
                }
                if(typeof articles[i] == undefined) {
                    console.log("error with undefined", articles);
                }
                var sentiment = parsed.sentiment.polarity;
                if(sentiment == 'undefined'){ // filter out those who can't be detected the right sentiment
                    iToDelete.push(i);
                } else {
                    articles[i].sentiment = sentiment;
                    if(articles[i].sentiment=="pos") articles[i].tree = tree_3.branches;
                    if(articles[i].sentiment=="neu") articles[i].tree = tree_2.branches;
                    if(articles[i].sentiment=="neg") articles[i].tree = tree_1.branches;

                }
            });
        })(i);
    };
    var n = 0;
    for (i = 0; i < iToDelete.length; i++) {
        articles.splice(iToDelete[i]-n,1);
        n++
    }

};

function evaluateArticles(){
    console.log("--> filter out articles, which don't provide enough material");
    var iToDelete = [];
    for (i = 0; i < articles.length; i++){
            if( articles[i].adjectives.length < 50 ){
                console.log("   --> remove article #" + i + " from the list: Not enough adjectives (<50)");
                iToDelete.push(i);

            } else if( articles[i].sentiment == undefined ){
                console.log("   --> remove article #" + i + " from the list: Problem recognising the sentiment");
                iToDelete.push(i);

            } else if( articles[i].title.length > 120 ){
                console.log("   --> remove article #" + i + " from the list: title too long");
                iToDelete.push(i);
            };
    };
    for (i = iToDelete.length; i >= 0 ; i--) {
        articles.splice(iToDelete[i],1);
    }
};


var createTree = function(){
    indexArticles = (indexArticles+1)%articles.length;
    var index = indexArticles;//Math.ceil(Math.random() * articles.length);
    console.log("--> create tree from article " +index);

    var tree = {};
    tree.title = articles[index].title;
    tree.branches = articles[index].tree;
    var numBranches = Object.keys(tree.branches).length;

    console.log(numBranches)
    console.log(articles[index].adjectives.length)
    for(i=0; i<numBranches; i++){
        tree.branches[i].adjective = articles[index].adjectives[i];
    }

    tree.adjectives = articles[index].adjectives;

    // console.log(tree);

    return tree;
}

function showOnConsole() {
    console.log('');
    console.log('ARTICLES');
    console.log('------');
    for (i = 0; i < articles.length; i++) {
        console.log('TITLE (' + articles[i].title.length + '): ' + articles[i].title);
        //console.log('TEXT : ' + articles[i].text);
        console.log('ADJECTIVES #' + articles[i].adjectives.length + ': ' + articles[i].adjectives);
        console.log('SENTIMENT : ' + JSON.stringify(articles[i].sentiment, 0, 4));

        console.log('------');
    }

}
