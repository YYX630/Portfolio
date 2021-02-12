import React, {useState} from 'react';
import ReactDOM from 'react-dom';
import './game1.css';





function Square(props){
  return (
    <button className="square" onClick={props.onClick}>
      {props.value}
    </button>
  )
}



function Board (props){
  
  function renderSquare(i){
    return (
      <Square 
        value={props.squares[i]}
        onClick={()=>props.onClick(i)}
      />
    );
  };

  return (
      <div>
        <div className="board-row">
          {renderSquare(0)}
          {renderSquare(1)}
          {renderSquare(2)}
        </div>
        <div className="board-row">
          {renderSquare(3)}
          {renderSquare(4)}
          {renderSquare(5)}
        </div>
        <div className="board-row">
          {renderSquare(6)}
          {renderSquare(7)}
          {renderSquare(8)}
        </div>
      </div>
  );
}


function Historyblock ({moves, showhistory, onClick}){

  if (showhistory){
    return (
      <div>
          <button onClick={()=>onClick(0)} >履歴を隠す</button>
          <ol>{moves}</ol>
      </div>
    )
  } else {
    return (
      <div>
          <button  onClick={()=>onClick(1)} >履歴を見る</button>
      </div>
    ) 
  }

}


function Game1 (props) {
  const [history,setHistory] = useState([{squares: Array(9).fill(null)}])
  const [stepNumber, setStepNumber] = useState(0)
  const [xIsNext, setXIsNext] = useState(true);
  const [showhistory, setShow] = useState(0);


  const handleShow = (i) => {
    setShow(i)
  }


  const handleClick = (i) => {

    // 新しくコピー。
    let newHistory = history.slice(0, stepNumber+1);
    let current = newHistory[newHistory.length-1];
    let squares = current.squares.slice();
    if (calculateWinner(squares)|| squares[i]){
      return;
    }
    //  勝負がすでについていたら、もしくはすでに押されているマスを押しても何も起きない。
    
    // そうでなければ。
    
    // 新しく作成したコピーに変更を反映
    squares[i] = xIsNext ? 'X': 'O';



    // historyに新しいコピーを結合
    setHistory(newHistory.concat([{
      squares: squares,
    }]))
    setStepNumber(newHistory.length)
    setXIsNext(!xIsNext)

  }


  const jumpTo = (step)=>{
    setStepNumber(step)
    setXIsNext((step%2)=== 0 )
  }




    let current = history[stepNumber];
    let winner = calculateWinner(current.squares);

    const moves = history.map((step,move)=>{
      const desc = move ? 'Go to move #' + move : 'Go to game start';
      return (
        <li key={move}>
          <button onClick={()=>jumpTo(move)}> {desc} </button>
        </li>
      );
    });




    let status;
    if (winner) {
      status = 'Winner: ' + winner;
    } else {
      status = 'Next player: ' + (xIsNext ? 'X' : 'O');
    }


    return (
      <div className="game">
        <div className="game-board">
          <Board 
            squares={current.squares}
            onClick={(i)=>handleClick(i)}
          />
        </div>
        <div className="game-info">
          <div>{status}</div>
          <Historyblock moves ={moves} showhistory={showhistory} onClick={handleShow}/>
        </div>
      </div>
    );
}

function calculateWinner(squares) {
  const lines = [
    [0, 1, 2],
    [3, 4, 5],
    [6, 7, 8],
    [0, 3, 6],
    [1, 4, 7],
    [2, 5, 8],
    [0, 4, 8],
    [2, 4, 6],
  ];
  for (let i = 0; i < lines.length; i++) {
    const [a, b, c] = lines[i];
    if (squares[a] && squares[a] === squares[b] && squares[a] === squares[c]) {
      return squares[a];
    }
  }
  return null;
}

export default Game1;