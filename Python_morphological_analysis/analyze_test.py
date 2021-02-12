import MeCab
mecab = MeCab.Tagger()
import re
from collections import Counter

GNI = r'tweet.txt'
with open(GNI) as K:
    K_text = K.read()
 
#テキストデータの確認
# print(K_text)


# Mecab で形態素解析
K_parsed = mecab.parse(K_text)
 
#解析結果の確認
# print(K_parsed)


#行単位に分割
K_parsed_lines = K_parsed.split('\n')
 
#処理に使うリストを作成
K_parsed_words = []
K_words = []



#  ここまでおK
#各行のタブ（\t）を除去
for K_parsed_line in K_parsed_lines:
    K_parsed_words.append(re.split('[\t,]', K_parsed_line))
 


#名詞・一般に該当する単語をリストに格納
for K_parsed_word in K_parsed_words:
    if (    K_parsed_word[0] not in ('EOS', '') 
        and K_parsed_word[4] == '名詞-普通名詞-一般' ):
        
            K_words.append(K_parsed_word[0])


# print(K_parsed_words)


# 出現頻度上位15語を抽出して表示
K_counter = Counter(K_words)
for K_word, K_count in K_counter.most_common(15):
    print('%s : %s' % (K_word, K_count))
