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


function Article(title, text, adjectives, tree) {
    this.title = title;
    this.text = text;
    this.adjectives = adjectives;
    this.sentiment;
    this.tree = tree;
};

var articles = [];

// program
//     .version('0.0.1')
//     .option('-m --monitor', 'Live monitor the incomming articles')


// if (program.monitor) {
//     setTimeout(function() {
//setInterval(function() {
setTimeout(function() {
    grabNewArticles();

    setTimeout(function() {


        showOnConsole();
    }, 10000);

}, 500);
//}, 5000);

//     }, 10000);
// }

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
                // sort them to get the longest in the start
                adjectives.sort(function(a, b) {
                    return b.length - a.length
                });

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

function getSentiment(){
    console.log("--> get sentiment from the " + articles.length + " article-titles");
    for (i = 0; i < articles.length; i++) {
        (function(i) {
            // get sentiment from the title
            var string = articles[i].text
            sentiment( string ,function(parsed){
                 if(typeof string !=='string' || string.trim().length ==0 ){
                     return ;
                 }

                 articles[i].sentiment = parsed;

                 //console.log(string)
                 //console.log(JSON.stringify(parsed,0,4))
             });
        })(i);
    };

}

function showOnConsole() {
    console.log('');
    console.log('ARTICLES');
    console.log('------');
    for (i = 0; i < articles.length; i++) {
        console.log('TITLE: ' + articles[i].title);
        //console.log('TEXT : ' + articles[i].text);
        console.log('ADJECTIVES: ' + articles[i].adjectives);
        console.log('SENTIMENT : ' + JSON.stringify(articles[i].sentiment,0,4));

        console.log('------');
    }
}
