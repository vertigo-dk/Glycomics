var program = require('commander');


var WordPOS = require('wordpos'),
    wordpos = new WordPOS();

// Get data from The Guardian
var guardian = require('guardian-news');

guardian.config({
  apiKey : 'b4090503-4d27-4c0f-bf3e-e20b710a58fe'
});

function Article(title, text, adjectives, tree) {
    this.title = title;
    this.text = text;
    this.adjectives = adjectives;
    this.tree = tree;
};

var articles = [];
var adjectives = [];

// program
//     .version('0.0.1')
//     .option('-m --monitor', 'Live monitor the incomming articles')


// if (program.monitor) {
//     setTimeout(function() {
//setInterval(function() {
setTimeout(function(){
    grabNewArticles();
    setTimeout(function(){
        extractAdjectives();
        setTimeout(function(){
            console.log('');
            console.log('ARTICLES');
            console.log('------');

            showOnConsole();
        }, 1000);
    }, 5000);
}, 500);
//}, 5000);

//     }, 10000);
// }

function grabNewArticles(){
    guardian.content({
      section : 'world',
      'show-blocks' : 'all'
    }).then(function(response){
        console.log("--> gather articles from the quardian");
        articles = [];
        var results = response.response.results;
        for (i = 0; i < results.length; i++) {
                articles.push(new Article(results[i].webTitle, results[i].blocks.body[0].bodyTextSummary));
        };



    }, function(err){
      console.log(err);
    });
};


function extractAdjectives(){
    console.log("--> extract adjectives from the "+articles.length+" articles");
    for (i = 0; i<articles.length; i++){
        //articles[i].adjectives = i;
        console.log(wordpos.getAdjectives(articles[i].text)); //.then(function(results){
        //     //articles[i].addAdj(results);
        //     //adjectives[i] = results;
        //     articles[i].adjectives = results;
        //
        //     console.log(articles[i].adjectives);
        // }, function(err){
        //     articles[i].adjectives = i;
        // });
        // //articles[i].adjectives
    };
}

function showOnConsole(){
    for (i = 0; i<articles.length; i++){
        console.log('TITLE: ' + articles[i].title);
        //console.log('TEXT : ' + articles[i].text);
        console.log('ADJ #: ' + articles[i].adjectives);
        console.log('------');
    }
}
