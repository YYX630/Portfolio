library(neuralnet)
library(MASS)

normalize_data = function(data)  ## the args are the two sets
  
{ 
  ranges <- sapply(data, function(x) max(x)-min(x)) ## range calculation
  
  normalized_data <- data/ranges   # the normalizatio
  
  return(normalized_data)
}




data= airquality
data=na.omit(data)
data = subset(data, select = c(Ozone,  Wind, Temp))
data=data.frame(scale(data))
data = normalize_data(data)   ## applying the function




judge = function (rate){
  
  n_data = nrow(data)
  r_train = rate
  total = 0
  
  for (i in 1:10){
    train_id = sample(n_data, n_data*r_train)
    train_data = data[train_id,]
    test_data = data[-train_id,]
    
    train_result=neuralnet(Ozone~Wind+Temp, train_data, hidden=c(10,10), threshold = 0.2)
    result = predict(train_result, test_data)
    total = total + cor(as.vector(result), test_data[, ncol(test_data)])/10
    
  }
  
  print(total)  
  
}


judge(0.7)


