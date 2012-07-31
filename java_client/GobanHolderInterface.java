abstract interface GobanHolderInterface {
	abstract public void jumpToMove( int $viewMove, int $lastMove );
	abstract public void jumpToMove( HTree<SGF.Game.Move>.HNode<SGF.Game.Move> $node );
	abstract public HTree<SGF.Game.Move>.HNode<SGF.Game.Move> currentMove();
}

