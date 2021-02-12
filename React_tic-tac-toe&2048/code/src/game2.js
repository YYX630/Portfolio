import React, {useState} from 'react';
import ReactDOM from 'react-dom';
import './game1.css';


function Square(props){
    let value;
    if(props.value===0){
        value = null;
    } else {
        value = props.value;
    }

    return (
        <button className="square" onClick={props.onClick}>
        {value}
        </button>
    )
}


function Button(props){
    return (
        <div>
            <button  onClick={()=>props.onClick(0)}>
                ↑
            </button>
            <button onClick={()=>props.onClick(1)}>
                ↓
            </button>
            <button  onClick={()=>props.onClick(3)}>
                ←
            </button>
            <button  onClick={()=>props.onClick(2)}>
                →
            </button>
        </div>

    )
}


function Board (props){
    console.log(props.squares);
  function renderSquare(i,j){
    return (
      <Square 
        value={props.squares[i][j]}
      />
    );
  };

  return (
      <div>
        <div className="board-row">
          {renderSquare(0,0)}
          {renderSquare(0,1)}
          {renderSquare(0,2)}
          {renderSquare(0,3)}
        </div>
        <div className="board-row">
          {renderSquare(1,0)}
          {renderSquare(1,1)}
          {renderSquare(1,2)}
          {renderSquare(1,3)}
        </div>
        <div className="board-row">
          {renderSquare(2,0)}
          {renderSquare(2,1)}
          {renderSquare(2,2)}
          {renderSquare(2,3)}
        </div>
        <div className="board-row">
          {renderSquare(3,0)}
          {renderSquare(3,1)}
          {renderSquare(3,2)}
          {renderSquare(3,3)}
        </div>
      </div>
  );
}



function Game2 (props) {

    const [squares, setSquares] = useState([
        [0,0,0,0],
        [0,0,0,0],
        [0,0,0,0],
        [0,0,0,0],
    ])

    const handleClick = (key) => {

        let newSquares = [];
        for (let i =0; i<squares.length; i++){
            newSquares[i] = squares[i].slice();
        }
    
    
        if (key ===0) moveUp(newSquares);
        if (key ===1) moveDown(newSquares);
        if (key ===2) moveRight(newSquares);
        if (key ===3) moveLeft(newSquares);
        randomcell(newSquares);
        setSquares(newSquares);
    }

    const handleKeyDown = (e) =>{
        let newSquares = [];
        for (let i =0; i<squares.length; i++){
            newSquares[i] = squares[i].slice();
        }

        if(e.keyCode === 38) moveUp(newSquares);
        if(e.keyCode === 39) moveRight(newSquares);
        if(e.keyCode === 40) moveDown(newSquares);
        if(e.keyCode === 37) moveLeft(newSquares);

        randomcell(newSquares);
        setSquares(newSquares);
    }
    



    return (
      <div className="game" >
        <div className="game-board">
          <Board 
            squares={squares}
          />
        </div>
        <div className="game-info">
          <Button
            squares={squares}
            onClick={(key)=>handleClick(key)}
          />
          <button onKeyDown={(e) => handleKeyDown(e)}>キーボード入力モード</button>
        </div>
      </div>
    );
}

function moveUp(newSquares){
    for (let col = 0; col < 4 ; col++){
        for (let row = 1 ; row < 4; row++){
            if(newSquares[row][col] === 0){
                continue;
            } else {
                // newSquares[row][col]に数字が入っているとき

                // 上にかぶる数字あるか
                let i;
                let flg=0;
                for (i = row-1 ; i>=0; i=i-1){

                    if(newSquares[i][col]===0){
                        continue;
                    } else if(newSquares[i][col]!==newSquares[row][col]){
                        flg = 0;
                        // 上にあるのは同じ数字ではない
                        break;
                    } else if(newSquares[i][col]===newSquares[row][col]){
                        flg = 1;
                        // 上にあるのは同じ数字
                        break;
                    }
                }
                // 上にある数字とかぶるとき。
                if(flg===1){ 
                    newSquares[i][col] = newSquares[i][col]*2;
                    newSquares[row][col]=0;

                } else {
                    let tmp = newSquares[row][col];
                    newSquares[row][col] = 0;
                    newSquares[i+1][col] = tmp;
                    // 個々の操作の順番大事。逆にしてはいけない。消してから、写す。
                    // i+1 == rowの時があるから。
                 }
            }
        }

    }
}

function moveDown(newSquares){
    for (let col = 0; col < 4 ; col++){
        

        for (let row = 4-2 ; row >= 0; row--){
            if(newSquares[row][col] === 0){
                continue;
            } else {
                // newSquares[row][col]に数字が入っているとき

                //下にかぶる数字あるか
                let i;
                let flg=0;
                for (i = row+1 ; i<=4-1; i=i+1){
                    if(newSquares[i][col]===0){
                        continue;
                    } else if(newSquares[i][col]!==newSquares[row][col]){
                        flg = 0;
                        // 下にあるのは同じ数字ではない
                        break;
                    } else if(newSquares[i][col]===newSquares[row][col]){
                        flg = 1;
                        // 下にあるのは同じ数字
                        break;
                    }
                }

                // 下にある数字とかぶるとき。
                if(flg){
                    newSquares[i][col] = newSquares[i][col]*2;
                    newSquares[row][col]=0;
                } else {
                    // 下にある数字とかぶらないとき
                    let tmp = newSquares[row][col];
                    newSquares[row][col] = 0;
                    newSquares[i-1][col] = tmp;
                    // 個々の操作の順番大事。逆にしてはいけない。消してから、写す。
                    // i-1 == rowの時があるから。
                 }
            }
        }

    }
}


function moveLeft(newSquares){
    for (let row = 0; row < 4 ; row++){
        

        for (let col = 1 ; col < 4; col++){
            if(newSquares[row][col] === 0){
                continue;
            } else {
                // newSquares[row][col]に数字が入っているとき

                // 左にかぶる数字あるか
                let i;
                let flg=0;
                for (i = col-1 ; i>=0; i=i-1){
                    if(newSquares[row][i]===0){
                        continue;
                    } else if(newSquares[row][i]!==newSquares[row][col]){
                        flg = 0;
                        // 左にあるのは同じ数字ではない
                        break;
                    } else if(newSquares[row][i]===newSquares[row][col]){
                        flg = 1;
                        // 左にあるのは同じ数字
                        break;
                    }
                }

                // 左にある数字とかぶるとき。
                if(flg){
                    newSquares[row][i] = newSquares[row][i]*2;
                    newSquares[row][col]=0;
                    // limit = limit;
                } else {
                    //　上にある数字とかぶらない時のみlimitを使う。
                    let tmp = newSquares[row][col];
                    newSquares[row][col] = 0;
                    newSquares[row][i+1] = tmp;
                    // 個々の操作の順番大事。逆にしてはいけない。消してから、写す。
                    // i+1 == colの時があるから。
                 }
            }
        }

    }
}


function moveRight(newSquares){
    for (let row = 0; row < 4 ; row++){
        

        for (let col = 4-2 ; col >= 0; col--){
            if(newSquares[row][col] === 0){
                continue;
            } else {
                // newSquares[row][col]に数字が入っているとき

                //右にかぶる数字あるか
                let i;
                let flg=0;
                for (i = col+1 ; i<=4-1; i=i+1){
                    if(newSquares[row][i]===0){
                        continue;
                    } else if(newSquares[row][i]!==newSquares[row][col]){
                        flg = 0;
                        // 右にあるのは同じ数字ではない
                        break;
                    } else if(newSquares[row][i]===newSquares[row][col]){
                        flg = 1;
                        // 右にあるのは同じ数字
                        break;
                    }
                }

                // 右にある数字とかぶるとき。
                if(flg){
                    newSquares[row][i] = newSquares[row][i]*2;
                    newSquares[row][col]=0;
                } else {
                    // 下にある数字とかぶらないとき
                    let tmp = newSquares[row][col];
                    newSquares[row][col] = 0;
                    newSquares[row][i-1] = tmp;
                    // 個々の操作の順番大事。逆にしてはいけない。消してから、写す。
                    // i-1 == colの時があるから。
                 }
            }
        }

    }
}


function randomcell(newSquares){
    // 最後に。一回押すたびに、2をランダムに出現。
    let flg =1; 
    function getRandomint(min, max) {
        min = Math.ceil(min);
        max = Math.floor(max);
        return Math.floor(Math.random() * (max - min) + min); //The maximum is exclusive and the minimum is inclusive
      }

    while (flg){
        let a = getRandomint(0,4); 
        let b = getRandomint(0,4);
        if (0 === newSquares[a][b]){
            newSquares[a][b] = 2;
            break;
        }
        flg = flg + 1;

        if(flg>4*4*2){
            for(let i =0; i<4; i++){
                for(let j=0; j<4; j++){
                    newSquares[i][j]=0
                }
            }
            return -1;
        } 
    }
    return 0;
}

export default Game2;