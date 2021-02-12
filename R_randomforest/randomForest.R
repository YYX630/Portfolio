library(randomForest)
library(neuralnet)

normalize_data = function(data)  ## the args are the two sets
  
{ 
  ranges <- sapply(data, function(x) max(x)-min(x)) ## range calculation
  
  normalized_data <- data/ranges   # the normalizatio
  
  return(normalized_data)
}


#function for calculating accuracy
calculate = function  (total){
  right = 0
  all = 0
  for (i in 1:6){
    right = right + total[i,i]
    for(j in 1:6){
      all = all + total[i,j]
    }
  }
  accuracy = (right/all)
  return(accuracy)
}

#read data
train_set0 = read.table("./HAR_X_train.txt")
test_set0 = read.table("./HAR_X_test.txt")

train_labels0 = read.table("./HAR_y_train.txt")
names(train_labels0)="label"
test_labels0 = read.table("./HAR_y_test.txt")
names(test_labels0)="label"


#ormalize
train_set = scale(train_set0)
test_set = scale(test_set0)
#train_set = normalize_data(train_set)
#test_set = normalize_data(test_set)


train_labels = train_labels0
test_labels = test_labels0


#labels
train_labels[train_labels[,1]==1, 1] = "WALKING"
train_labels[train_labels[,1]==2, 1] = "WALKING_UPSTAIRS"
train_labels[train_labels[,1]==3, 1] = "WALKING_DOWNSTAIRS"
train_labels[train_labels[,1]==4, 1] = "SITTING"
train_labels[train_labels[,1]==5, 1] = "STANDING"
train_labels[train_labels[,1]==6, 1] = "LAYING"

test_labels[test_labels[,1]==1, 1] = "WALKING"
test_labels[test_labels[,1]==2, 1] = "WALKING_UPSTAIRS"
test_labels[test_labels[,1]==3, 1] = "WALKING_DOWNSTAIRS"
test_labels[test_labels[,1]==4, 1] = "SITTING"
test_labels[test_labels[,1]==5, 1] = "STANDING"
test_labels[test_labels[,1]==6, 1] = "LAYING"


#merge
train_data = data.frame(train_set, train_labels)
test_data = data.frame(test_set, test_labels)

train_labels_factor = as.factor(train_labels[,1])
test_lables_factor = as.factor(test_labels[,1])

n_testdata = nrow(test_data)
n_class = 6


#パラメータチューニング
rfTuning = tuneRF(x= train_set, y = train_labels_factor, stepFactor = 2, improve = 0.05, trace = TRUE, plot = TRUE, doBest = TRUE)


#初回実行
train_result=randomForest(train_labels_factor~., train_set, mtry = 60, ntree=1501)

#test
result = predict(train_result, test_data)
total = matrix(rep(0, n_class^2), nrow = n_class)
total = total + table(test_data[, ncol(test_data)], result) 

#表示
print(total)
print(calculate(total))

#########################################################################################################################
#特徴量抽出
IMPORTANTFACTOR = importance(train_result)
train_set_filtered = train_set[,IMPORTANTFACTOR > 2]
test_set_filtered  = test_set[, IMPORTANTFACTOR > 2]
train_data_filtered = data.frame(train_set_filtered, train_labels)
test_data_filtered = data.frame(test_set_filtered, test_labels)



#パラメータ調整後randomfores実行
train_result=randomForest(train_labels_factor~., train_set_filtered, mtry = 60, ntree=1501)


#test
result = predict(train_result, test_data_filtered)
total = matrix(rep(0, n_class^2), nrow = n_class)
total = total + table(test_data_filtered[, ncol(test_data_filtered)], result) 

#表示
print(total)
print(calculate(total))



