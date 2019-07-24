import java.util.Scanner;

public class GameState {
	private final int YMARGIN = 100; 
	public class PlayerState {
		private Point position;
		private Point velocity;
		public PlayerState() {}
		public void update(Scanner in) {
			position = new Point();
			velocity = new Point();
			position.setX(in.nextInt());
			position.setY(in.nextInt());
			velocity.setX(in.nextInt());
			velocity.setY(in.nextInt());
		}
		public Point getPosition() {
			return position;
		}
		public Point getVelocity() {
			return velocity;
		}
	}
	private int remainingTime;
	private int step;
	private final int stepLimit;
	private final int width, length;
	private final int visionLimit;
	private int obstacle[][];
	private PlayerState playerState, opponentState;
	private boolean hasMoved;
	
	public GameState(Scanner in) {
		remainingTime = in.nextInt();
		step = 0;
		stepLimit = in.nextInt();
		width = in.nextInt();
		length = in.nextInt();
		visionLimit = in.nextInt();
		obstacle = new int[width][length+YMARGIN]; 
		for(int x = 0;x < width; x++) {
			for(int y = 0;y < length+YMARGIN; y++) {
				obstacle[x][y] = 0;
			}
		}
		playerState = new PlayerState();
		opponentState = new PlayerState();
		hasMoved = false; 
	}

	public void update(Scanner in) {
		step = in.nextInt();
		remainingTime = in.nextInt();
		Point lastPosition = playerState.getPosition();
		playerState.update(in);
		// to avoid out-of-view obstacles
		if(lastPosition != null && !isObstacle(lastPosition.add(playerState.getVelocity())) && !playerState.getPosition().equals(lastPosition.add(playerState.getVelocity()))) {
			hasMoved = false;
		}else {
			hasMoved = true;
		}
		opponentState.update(in);
		for(int y = playerState.getPosition().getY() - visionLimit; 
				y <= playerState.getPosition().getY() + visionLimit; y++) {
			for(int x = 0; x < width; x++) {
				int s = in.nextInt();
				if(y >= 0 && y < length+YMARGIN) {
					obstacle[x][y] = s;
				}
			}
		}
	}
	
	public void printCurrentState() {
		System.err.println("=========== Step: "+ step +" / " + stepLimit + " ==================");
		for(int y = playerState.getPosition().getY() + visionLimit; 
					y >= playerState.getPosition().getY() - visionLimit; y--) {
			for(int x = 0; x < width; x++) {
				if(x == playerState.getPosition().getX() && y == playerState.getPosition().getY()) {
					System.err.print("p");
				}else if(x == opponentState.getPosition().getX() && y == opponentState.getPosition().getY()) {
					System.err.print("o");
				}else if(isObstacle(x, y)) {
					System.err.print("*");
				}else {
					System.err.print(" ");
				}
			}
			System.err.println("");
		}
		System.err.println("Velocity=("+ playerState.getVelocity().getX() +", "+playerState.getVelocity().getY()+")");
		System.err.println("Remaining Time: "+ remainingTime +" [mu_sec]");
	}
	
	public boolean isObstacle(int x, int y) {
		if(x < 0 || x >= width || y < 0) {
			return true;
		}
		return obstacle[x][y] == 1;
	}
	
	public boolean isObstacle(Point p) {
		return isObstacle(p.getX(), p.getY());
	}
	
	private boolean hasObstacleCollision(Point currentPosition, Point velocity) {
		Point nextPosition = currentPosition.add(velocity);
		if(isObstacle(currentPosition) || isObstacle(nextPosition)) {
			return true;
		}
		if(velocity.getX() == 0) {
			if(velocity.getY() == 0) {
				return false;
			}
			int yStep = velocity.getY() < 0 ? -1 : 1;
			for (int cy = currentPosition.getY(); cy != nextPosition.getY(); cy += yStep) {
				if(isObstacle(currentPosition.getX(), cy)) {
					return true;
				}
			}
			return false;
		}
		if(velocity.getY() == 0) {
			int xStep = velocity.getX() < 0 ? -1 : 1;			
			for (int cx = currentPosition.getX(); cx != nextPosition.getX(); cx += xStep) {
				if(isObstacle(cx, currentPosition.getY())) {
					return true;
				}
			}
			return false;
		}
		double dxStep = velocity.getX() / (double)Math.abs(velocity.getY());
		int yStep = velocity.getY() < 0 ? -1 : 1;
		double cx = currentPosition.getX();
		for (int cy = currentPosition.getY(); cy < nextPosition.getY(); cx += dxStep, cy += yStep) {
			int fcx = (int)Math.floor(cx);
			int ccx = (int)Math.ceil(cx);
			double nextX = cx + dxStep;	
			int ny = cy + yStep;
			int fnx = (int)Math.floor(nextX);
			int cnx = (int)Math.ceil(nextX);
			if(isObstacle(fnx, ny) && isObstacle(cnx, ny)){
				return true;
			}
			if(fcx == fnx || ccx == cnx) {
				if (fnx != cnx) {
					if ((isObstacle(fnx, ny) && isObstacle(cnx, cy)) ||
							(isObstacle(fnx, cy) && isObstacle(cnx, ny)) ) {
						return true;	
					}
				} else {
					if ((isObstacle(fcx, ny) && isObstacle(ccx, cy)) ||
							(isObstacle(fcx, cy) && isObstacle(ccx, ny)) ) {
						return true;	
					}
				}
			} else {
				if(dxStep > 0) {
					for(int start = fcx + 1; start <= cnx; start++) {
						if(!isObstacle(start, cy))continue;
						for(int end = start - 1; end <= start + 1 && end < cnx; end++) {
							if(isObstacle(end, ny)) {
								return true;
							}
						}
					}
				} else {
					for(int start = ccx - 1; start >= fnx; start--) {
						if(!isObstacle(start, cy))continue;
						for(int end = start + 1; end >= start - 1 && end > fnx; end--) {
							if(isObstacle(end, ny)) {
								return true;
							}
						}
					}
				}
			}
		}
		return false;
	}
	
	public boolean hasPriority(Point position, Point opponentPosition) {
		if(position.getY() < opponentPosition.getY()) {
			return true;
		}else if(position.getY() == opponentPosition.getY() && position.getX() < opponentPosition.getX()) {
			return true;
		}
		return false;
	}
	
	public boolean hasOpponentCollision(Point position, Point velocity, Point opponentPosition) {
		int min, max;
		if(velocity.getX() < 0) {
			min = position.getX() + velocity.getX();
			max = position.getX();
		}else {
			min = position.getX();
			max = position.getX() + velocity.getX();
		}
		if(min <= opponentPosition.getX() && opponentPosition.getX() <= max && 
				Math.abs(velocity.getY()*(opponentPosition.getX() - position.getX()) 
						- velocity.getX() * (opponentPosition.getY() - position.getY())) < 1e-10) {
			// collision on the opponent's start point
			return true;
		}
		if(hasPriority(position, opponentPosition)) {
			return false;
		}
		//TODO: predict moves
		return false;
	}

	public boolean isPlayable(Point accel) {
		return isPlayable(playerState.getPosition(), playerState.getVelocity(), accel, opponentState.getPosition());
	}
	
	public boolean isPlayable(Point position, Point velocity, Point accel, Point opponentPosition) {
		Point nextVelocity = velocity.add(accel);
		if(nextVelocity.getY() < 0) {
			// no need to go back
			return false;
		}
		return hasMoved && !hasObstacleCollision(position, nextVelocity) && !hasOpponentCollision(position, nextVelocity, opponentPosition) ;
	}
	
	public PlayerState getPlayerState() {
		return playerState;
	}
	
	public PlayerState getOpponentState() {
		return opponentState;
	}
	
}
