import numpy as np
import matplotlib as plt
# 時系列データ=横ベクトル
# X = 行列


def E(X):
    a = np.mean(X, axis=1)
    b = a.reshape([len(a), 1])
    return b


def kurtosis(y):  # y:1×N の横ベクトル
    return np.mean(y**4)-3*np.mean(y**2)*2


def Covariance(X):
    A = X@X.T
    N = len(X[0])
    return A/N


def centralize(X):
    return X - E(X)


def whitening(X):
    # Xは行列
    #　すでに平均は0

    #　共分散行列
    Sigma = Covariance(X)

    #　白色化
    lam, E = np.linalg.eig(Sigma)  # 固有値、固有ベクトル
    lam_inv_root = [1/(i)**(0.5) for i in lam]
    D_inv_root = np.diag(lam_inv_root)
    V = E@D_inv_root@E.T
    Z = V@X
    return Z


def normalize(w):  # w=縦ベクトル
    norm = np.linalg.norm(w, 2)
    return w/norm


def standardize_vec(v, newmin, newmax):
    a = (newmax - newmin)/(max(v) - min(v))
    b = newmax - a*max(v)
    ans = [a*i + b for i in v]
    ans = np.array(ans)
    return ans


def standardize(Y):
    n = len(Y)
    ans = []
    for i in range(n):
        vec = standardize_vec(Y[i][0], -1, 1)
        ans.append(vec)
    return ans


def func(w, Z):
    g = (w.T@Z)**3
    f = Z*g
    return E(f)-3*w


def optimize(w, Z):
    # wは列ベクトル、　Zは行列
    w = normalize(w)
    old_kurtosis = kurtosis(w.T@Z)
    while True:
        new_w = func(w, Z)
        new_w = normalize(new_w)
        new_kurtosis = kurtosis(new_w.T@Z)

        # kurtosisの絶対値の差を評価
        delta = abs(abs(new_kurtosis)-abs(old_kurtosis))

        w = new_w
        old_kurtosis = new_kurtosis
        if (delta < 0.000001):
            break

    return w


def difference(W):
    n = len(W)
    delta = 0
    for i in range(n):
        for j in range(i+1, n):
            delta = delta + np.linalg.norm(W[i]-W[j], ord=2)
    return delta


def best_W(ans):
    norm = [difference(W) for W in ans]
    max_index = np.argmax(norm)
    return ans[max_index]


def autoICA(X):
    X = centralize(X)
    Z = whitening(X)
    n, d = Z.shape
    ans = []
    for j in range(10):
        W = []
        for i in range(n):
            # 適当な初期値
            l = [np.random.randint(1, 101) for i in range(n)]
            w = np.array([l]).T  # 縦ベクトルw
            w = optimize(w, Z)
            # if(i == 0 or abs(w[0][0] - W[i-1][0][0]) > 0.8):
            W.append(w.T)
        ans.append(W)

    # 一番離れてるWを採用
    W = best_W(ans)
    Y = W@Z
    Y = standardize(Y)

    return Y


def manualICA(X, given_W):
    X = centralize(X)
    Z = whitening(X)
    n, d = Z.shape

    W = []
    for i in range(n):
        # 初期値はgiven
        w = given_W[i].reshape([n, 1])  # 縦ベクトルw
        w = optimize(w, Z)
        W.append(w.T)

    Y = W@Z
    Y = standardize(Y)

    return (Y, W)
