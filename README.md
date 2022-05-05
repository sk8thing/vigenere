# Description
Application that cracks the [Vigenere cipher](https://en.wikipedia.org/wiki/Vigen%C3%A8re_cipher) using coincidence frequency analysis in order to find the key length and frequency analysis of letters for finding the key itself. 
Each language has a different letter frequency distribution, the graph below shows the frequency of each letter in Romanian, which is the language the application was written for.
![1](https://user-images.githubusercontent.com/101511232/158178475-fe39e0a2-8152-412a-8db8-62578875636c.png)
## Running the application
You can download the executable from the release tab, or you can compile it yourself. 
To run the application you need to create a text file containing the cipher text. 
The cipher text should only contain letters from A-Z, diacritics, punctuation marks and other symbols should be unencrypted. 
The length of the cipher text highly affects the success rate of decrypting, it should be around 200 characters minimum including whitespaces for a high probability of decrypting. 
After creating the file, you can simply do (providing a key argument only decrypts the text using it)
```
./cvigenere.exe path/to/example.txt key
```
## How it works
The cryptanalysis of the Vigenere cipher is split up in 3 steps:
* Finding the key length
* Splitting the cipher text
* Using frequency analysis on each part
### Finding the key length
In order to find the key length, all the whitespaces from the cipher text have to be removed.
A copy of the modified cipher text is created and it gets shifted to the right by 1.
After that we start counting the letters that are the same in both strings.
This process repeats until the two strings don't overlap.
The reason we do this is that the key repeats itself and when that happens there's a high chance same letters are encrypted using the same letter of the key.
Here's an example where the string "ABCABCABC" is encrypted using the key "KEY":
```
KEYKEYKEY
KFAKFAKFA
 KFAKFAKFA - no coincidences
 
KEYKEYKEY
KFAKFAKFA
  KFAKFAKFA - no coincidences
  
KEYKEYKEY
KFAKFAKFA
   KFAKFAKFA - lots of coincidences
```
This is just an example.
What happens in a real scenario is that even though the key repeats itself, there isn't a spike in the number of coincidences.
That's why the success rate highly depends on the length of the cipher text.
Keep in mind that there can also be false positive results, when there's a high number of coincidences but the key doesn't repeat.
The way the length of the key is determined is by looking for coincidence spikes that occur at same distance.
### Splitting the cipher text
Once we found the length of the key, we split the cipher text into N parts, where N is the key length.
This way the first part contains the letters encrypted with the first letter of the key, the second part contains the letters encrypted with the second letter of the key and so on.
The result is a bunch of strings encrypted with the same key, that can be easily decrypted using letter frequency analysis.
### Using frequency analysis on each part
In this final step we run a letter frequency analysis on each part to find each letter of the key.
This is done in the same way the [Caesar cipher](https://github.com/sk8thing/caesar) is cracked.
All the letters inside a part are decrypted using all 25 possible shifts and each time the result gets assigned a letter frequency score.
The one with the highest score is considered the correct letter of the key and this repeats until we find the key.
Finally we decrypt the cipher text using the key.