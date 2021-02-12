window.Seed = (function () {
  function generateVoteCount() {
    return Math.floor((Math.random() * 50) + 15);
  }

  const products = [
    {
      id: 1,
      title: 'ジョンスノウ',
      description: '「我は人々の領土を守り抜く盾なり」',
      url: '#',
      votes: generateVoteCount(),
      submitterAvatarUrl: 'images/avatars/daniel.jpg',
      productImageUrl: 'images/products/jhon.jpg',
    },
    {
      id: 2,
      title: 'デナーリス',
      description: '「今度私に手をあげるようなことがあったら　そのときはあなたの手がないと思え」',
      url: '#',
      votes: generateVoteCount(),
      submitterAvatarUrl: 'images/avatars/kristy.png',
      productImageUrl: 'images/products/デナーリス.jpg',
    },
    {
      id: 3,
      title: 'ティリオン',
      description: '「俺がお前が思うようなモンスターだったらよかったのに」',
      url: '#',
      votes: generateVoteCount(),
      submitterAvatarUrl: 'images/avatars/veronika.jpg',
      productImageUrl: 'images/products/ティリオン.jpg',
    },
    {
      id: 4,
      title: 'ジェイミー',
      description: '「愛のためだ」',
      url: '#',
      votes: generateVoteCount(),
      submitterAvatarUrl: 'images/avatars/molly.png',
      productImageUrl: 'images/products/ジェイミー.jpg',
    },
  ];

  return { products: products };
}());
