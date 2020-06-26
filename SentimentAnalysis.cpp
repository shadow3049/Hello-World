#include <algorithm>
#include <sstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include "stop.cpp"

using std::vector;
using std::string;

class LexiconDictionary
{
protected:
    std::vector<std::pair< std::string, float>> listOfWords;
    LexiconDictionary()
    {
        std::ifstream fp;
        fp.open("lexicon.txt");
        std::string word, value;
        for(size_t t = 0; t < 7514 ; ++t) 
        {
            fp >> word;
            fp >> value;

            listOfWords.push_back( std::make_pair(word, std::stof(value)) );

            for(size_t j = 0; j < 11; ++j)
                fp >> word;
        } 
        fp.close();
        std::sort(listOfWords.begin(), listOfWords.end());
    }
};

class GetText
{
protected:
	string textToAnalyze, intermediate;
	GetText(string filePath)
	{
		std::ifstream FileToRead;
        FileToRead.open(filePath);
        while (FileToRead >> intermediate)
        	textToAnalyze = textToAnalyze + ' ' + intermediate;
	}
	
};

class tokeniser: protected GetText
{
public:
	vector <string> tokens; 

	tokeniser (string filePath) : GetText (filePath) 
	{

		for (std::string::iterator it = textToAnalyze.begin(); it != textToAnalyze.end(); ++it)
  		{
  			if(isalnum(*it)||*it == '-' || *it == '\'');
  			else *it = ' ';
 		}

 		transform(textToAnalyze.begin(), textToAnalyze.end(), textToAnalyze.begin(), ::tolower);

  		std::stringstream check1(textToAnalyze); 

    	while(check1 >> intermediate)  
       		tokens.push_back(intermediate); 

    	std::sort(tokens.begin(), tokens.end());
	}
};

class removeStopWords: protected StopWords, public tokeniser
{
public:
	size_t i = 0;
	removeStopWords(string filePath) : tokeniser (filePath)
	{
		for (std::vector<string>::iterator it = tokens.begin(); it != tokens.end(); ++it)
		{
			while(*it >= stopwords[i])
			{
				if(*it == stopwords[i])
					tokens.erase(it--);
				else
					++i;
			}
		}
	}
};

class findSentimentScore: public removeStopWords, protected LexiconDictionary
{
public:
	float negativeScore = 0.0, positiveScore = 0.0;
	size_t i = 0;
	findSentimentScore(string filePath) : removeStopWords (filePath)
	{
		for (std::vector<string>::iterator it = tokens.begin(); it != tokens.end(); ++it)
		{
			while(*it >= listOfWords[i].first)
			{
				if(*it == listOfWords[i].first)
				{
					if(listOfWords[i].second > 0)
						positiveScore += listOfWords[i].second;
					else
						negativeScore -= listOfWords[i].second;
					tokens.erase(it--);
				}
				else
					++i;
			}
		}
	}
	friend std::ostream &operator << ( std::ostream &output, const findSentimentScore &F )
	{ 
    	if(F.negativeScore > F.positiveScore)
    	{
    		output << F.negativeScore * 100 / (F.negativeScore + F.positiveScore) << "% Negative\n";
    	}
    	else if(F.negativeScore < F.positiveScore)
    	{
    		output << F.positiveScore * 100 / (F.negativeScore + F.positiveScore) << "% Positive\n";
    	}
    	else 
    		output << "Nuetral review\n";
    	return output;            
}
};

int main() 
{
	string filepath;
	std::getline(std::cin, filepath);
	findSentimentScore F(filepath);
	std::cout << F;
  	return 0;
}
