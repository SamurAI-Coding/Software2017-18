import java.util.Random;
import java.util.Scanner;
import java.util.Vector;

public class RandomPlayer {
	public Random rand;
	public RandomPlayer() {
		rand = new Random(0);
	}	
	
	public Point play(GameState state) {
		Vector<Point> candidates = new Vector<Point>();
		for(int y = -1; y < 2; y++) {
			for(int x = -1; x < 2; x++) {
				Point accel = new Point(x, y);
				if(state.isPlayable(accel)) {
					candidates.add(accel);
				}
			}
		}
		if(candidates.size() == 0) {
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
			candidates.add(new Point(cx, cy));
		}
		System.err.print("Candidates:");
		for(Point candidate:candidates) {
			System.err.print("("+candidate.getX()+", "+candidate.getY()+"), ");
		}
		System.err.println();
		return candidates.get(rand.nextInt(candidates.size()));
	}

	
	public static void main(String[] args) {
		Scanner in = new Scanner(System.in);
		RandomPlayer player = new RandomPlayer();
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
