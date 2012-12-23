abstract interface GobanHolderInterface {
	abstract public void setMoveSlider( int $viewMove, int $lastMove );
	abstract public void jumpToMove( HTree<SGF.Move>.HNode<SGF.Move> $node );
	abstract public HTree<SGF.Move>.HNode<SGF.Move> viewMove();
	abstract public HTree<SGF.Move>.HNode<SGF.Move> currentMove();
	abstract public void updateSetup();
}

