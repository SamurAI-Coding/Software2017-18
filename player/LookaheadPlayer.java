import java.util.Scanner;

public class LookaheadPlayer {
	private final int SEARCH_DEPTH = 4;
	public LookaheadPlayer() {}	
	
	private int playRecursive(GameState state, Point position, Point velocity, Point opponentPosition, int dist, int depth, Point bestAccel) {
		if(depth == 0) {
			int nmoves = 0;
			for(int y = 1; y > -2; y--) {
				for(int x = -1; x < 2; x++) {
					Point accel = new Point(x, y);
					if(state.isPlayable(position, velocity, accel, opponentPosition)) {
						nmoves++;
					}
				}
			}
			// prefer large Y, large move, many moves
			return position.getY() * 100 + dist * 20 + nmoves;
		}
		int bestScore = Integer.MIN_VALUE;
		for(int y = -1; y < 2; y++) {
			for(int x = -1; x < 2; x++) {
				Point accel = new Point(x, y);
				if(state.isPlayable(position, velocity, accel, opponentPosition)) {
					Point tmpAccel = new Point();
					int score = playRecursive(state, position.add(velocity).add(accel), velocity.add(accel), opponentPosition, dist + Math.abs(velocity.getX()+x)+Math.abs(velocity.getY() + y), depth - 1, tmpAccel);
					if(score > bestScore) {
						bestAccel.setX(x);
						bestAccel.setY(y);
						bestScore = score;
					}else if(score == bestScore) {
						if(bestAccel.getY() < y) {
							bestAccel.setX(x);
							bestAccel.setY(y);
						}else if(bestAccel.getY() == y && Math.abs(velocity.getX() + bestAccel.getX()) < Math.abs(velocity.getX() + x)) {
							bestAccel.setX(x);
						}
					}
				}
			}
		}
		return bestScore;
	}
	
	public Point play(GameState state) {
		Point bestAccel = new Point(-2, -2);
		int score = playRecursive(state, state.getPlayerState().getPosition(), state.getPlayerState().getVelocity(), state.getOpponentState().getPosition(), 0, SEARCH_DEPTH, bestAccel);
		if(score < 0) {
			// no way to avoid collision ... slow down
			int cx = 0, cy = 0;
			if(state.getPlayerState().getVelocity().getX() > 0) {
				cx = -1;
			} else {
				cx = 1;
			}
			if(state.getPlayerState().getVelocity().getY() > 0) {
				cy = -1;
			}else {
				cy = 1;
			}
			bestAccel = new Point(cx, cy);
		}
		return bestAccel;
	}
	
	public static void main(String[] args) {
		Scanner in = new Scanner(System.in);
		LookaheadPlayer player = new LookaheadPlayer();
		GameState state = new GameState(in);
		System.out.println(0);
		while(true) {
			state.update(in);
			state.printCurrentState();
			Point p = player.play(state);
			System.out.println(p.getX()+" "+p.getY());
		}
	}


}
