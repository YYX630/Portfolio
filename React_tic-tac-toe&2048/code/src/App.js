import React, {useState} from 'react';
import Game1 from './game1';
import Game2 from './game2';
import './css/App.css'; 

function App(){

  const [game, setGame] = useState(0);
  

  
  if (game===0){
    return (
      <div className="App-header">
        <h1 >Start A New Game</h1>
        <div class="btn-border-gradient-wrap">
          <button className="btn btn-border-gradient" onClick={()=>{setGame(1)}}>三目並べ</button>
        </div>
        <br/>
        <div class="btn-border-gradient-wrap">
          <button className="btn btn-border-gradient" onClick={()=>{setGame(2)}}>2048</button>
        </div>
      </div>
    )
  } else if (game ===1){
    return (
      <div className="App-header">
      <h1 >三目並べ</h1>
      <div class="btn-border-gradient-wrap">
      <button href="" className="btn btn-border-gradient" onClick={()=>{setGame(0)}}>ホームに戻る</button>
      </div>
      <br/>
      <Game1/>
      </div>
    )
  } else if (game ===2){
    return (
      <div className="App-header">
      <h1 >2048</h1>
      <div class="btn-border-gradient-wrap">
        <button className="btn btn-border-gradient" onClick={()=>{setGame(0)}}>ホームに戻る</button>
      </div>
      <br/>
      <Game2/>
      </div>
    )

  }


  }

export default App;