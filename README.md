## The problem

The problem was to solve whether in a given sentence the mouse is a computer device or an animal. 


## Idea

The main idea for this program was to:

1. Create a dictionary of word probabilities being in either of the contexts.
2. For each sentence, create a histogram of probabilities based on occuring words.
3. Feed the neural network with normalized histograms and normalized sentence lengths.

## Dataset 

For now the dataset is in CSV format, where columns are separated by semicolons.

The dataset is assumed to be labeled for the probability of being in animal context. 
For the challenge.cpp, the assumption is that animal context and computer context exclude each other.
```csv
sentence; animal_prob
I love my mouse!; 0.9
```

The assumption is negated for the challenge_complex.cpp.The words corresponding to a given context are also given.
```csv
sentence; animal_words; animal_prob; computer_words; computer_prob
Catch that mouse !;Catch;0.7;Catch;0.3
```



## Dictionary of words and probabilities.

We create a map of lower-case words as keys with probability histograms as values. When a sentence is divided into words, each words' histograms are updated by adding an element to the bin corresponding to the sentence's probability. If the probability is 0.71, we update the word's bin corresponding to 0.7-0.8 values.

After traversing all the sentences, we choose a median value for each word.


## Histograms as input for the neural network

Before feeding the sentence to a neural network, we first decompose it into words, and then we lookup for the words' probalities. We put these probalities into a separate probability histogram, which we first normalise and then put into the neural network. 

## Neural Network

The neural network is trained with the backpropagation algorithm with cross-entropy cost function.


## Evaluation

For now the sample dataset has 15 sentences. More samples are needed to properly evaluate the algorithm.

## Technicial details

The program uses [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page) (MPL2), [RapidCheck](https://github.com/emil-e/rapidcheck) (BSD-2) and [Fast C++ CSV Parser](https://github.com/ben-strasser/fast-cpp-csv-parser) (BSD-3).

## Future work

* Create a separate neural network for predicting word probabilities.

* Experiment with other cost functions.

