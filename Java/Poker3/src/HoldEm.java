import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.EnumMap;
import java.util.HashMap;

public class HoldEm {
	
	Player[] players;
	Card[] board;	
	
	public HoldEm(Player[] inputPlayers, Card[] inputBoard){
		
		this.players = inputPlayers;
		this.board = inputBoard;
	}
	
	public HoldEm(HoldEm inputHoldEm){
		this.players = inputHoldEm.players;
		this.board = inputHoldEm.board;
	}
	
	public String toString(){
		StringBuilder str = new StringBuilder("PLAYERS\n-----------------\n");
		for (Player player: this.players){
			for (Card card: player.hand){
				if (card == null){
					str.append("NULL");
				}
				else{
					str.append(card.symbol+" "+card.suit);
				}	
				str.append(" ");
			}
			str.append("\n");
		}
		str.append("\nBOARD\n------------------\n");
		str.append(Player.printCardArray(this.board));
		
		return str.toString();
	}
	
	
	
	private static void resetIntArray(int[] arr){
		for (int i = 0; i < arr.length; i++){
			arr[i] = 0;
		}
	}
	
	private static void resetDoubleArray(double[] arr){
		for (int i = 0; i < arr.length; i++){
			arr[i] = 0;
		}
	}
	
	private static void resetCardArray(Card[] arr){
		for (int i = 0; i < arr.length; i++){
			arr[i] = null;
		}
	}
	
	public static void finishBoard(Card[] entireHand, int size, ArrayList<Card> deck){
		
		
		int numCardsLeft = deck.size();
		for (int i = 0; i < size; i++){
			
			int n = (int)(Math.random()*numCardsLeft);
			entireHand[i] = deck.remove(n);
			numCardsLeft--;
		}	
	}
	
	public static void finishHand(Card[] holeCards, ArrayList<Card> deck){
		
		int numCardsLeft = deck.size();
		for (int i = 0; i < 2; i++){
			if (holeCards[i] == null){
				int n = (int)(Math.random()*numCardsLeft);
				holeCards[i] = deck.remove(n);
				numCardsLeft--;
			}
		}
	}
	
	public void getWinPercentages(double[] winArray, double[]tieArray){

		int iterations = 50000;
		EnumMap<Symbol, Integer> valueCount = new EnumMap<Symbol, Integer>(Symbol.class);
		EnumMap<Suit, Integer> suitCount = new EnumMap<Suit, Integer>(Suit.class);
		int[] valueList = new int[7];
		this.getWinPercentagesHelper(winArray, tieArray, valueCount, suitCount, valueList, iterations);
	}
	
	private void getWinPercentagesHelper(double[] winArray, double[]tieArray,	EnumMap<Symbol, Integer> valueCount, 
			EnumMap<Suit, Integer> suitCount, int[] valueList, int iterations){
		// Set up deck and HashMaps
		ArrayList<Card> deck = Player.getFullDeck();
		int boardSize = this.board.length;		
		int numCardsToAdd = 5 - boardSize;
		for (Player player: this.players){
			for (Card card: player.hand){
				if (card != null){
					deck.remove(card);
				}
			}
		}
		
		for (Card card: this.board){
			deck.remove(card);
		}
		
		// Initialize data structures
		int[] findTies = new int[this.players.length];		
		Card[] holeCards = new Card[2];	
		Card[] entireHand = new Card[7];
		Hand myHand = new Hand(0,0,new int[0]);
		Hand bestHand = new Hand(0,0,new int[0]);
		ArrayList<Card> temp = new ArrayList<Card> (deck);	// So we can reset deck after dealing/turning cards
		//Start main loop
		
		for (int i = 0; i < iterations; i++){		
			
			// Initializations
			
			// HoldEm.resetCardArray(entireHand);
			HoldEm.finishBoard(entireHand, numCardsToAdd, deck);
			if (boardSize != 0){
				System.arraycopy(this.board, 0, entireHand, numCardsToAdd, boardSize);
			}
				
			HoldEm.resetIntArray(findTies);
			int tieIndex = 0;
			bestHand.setHand(0, 0, new int[0]);
			int bestPlayerPos = 0;
			
			
			for (Player player: this.players){		
				
				// Add hole cards if player's hand is empty
				// HoldEm.resetCardArray(holeCards);						
				System.arraycopy(player.hand, 0, holeCards, 0, 2);
				
				if (holeCards[1] == null){							
					HoldEm.finishHand(holeCards, deck);
				}				
				
				
				System.arraycopy(holeCards, 0, entireHand, numCardsToAdd+boardSize, 2);	// to here
				
				// What hand does the player have
				
				// Should be able to initialize most of this before main loop
				player.getValueCount(valueCount, entireHand);	
				player.getSuitCount(suitCount, entireHand);	
				
				player.getHand(myHand, valueCount, suitCount, valueList, entireHand);		
//				System.out.println(myHand);
				// Find best hand or ties for best hand
				int comparison = myHand.compareTo(bestHand);

				if (comparison > 0){
					
					bestHand.copyHand(myHand);
					bestPlayerPos = player.position;
					
					
					HoldEm.resetIntArray(findTies);
					findTies[0] = player.position;
					tieIndex = 1;
				}
				else if(comparison == 0){
					
					findTies[tieIndex] = player.position;
					tieIndex++;
				}
				
				
			}
			

			
			
			
			// If there is a tie, find it
			if (findTies[1] != 0){
				
				for (int k = 0; k < findTies.length; k++){
					if (k > 1 && findTies[k] == 0){
						break;
					}
					
					// increase tieCount of each player in tieArray
					tieArray[findTies[k]]++;
				}
			}
			//otherwise, increase winCount of bestPlayer
			else{
				
				winArray[bestPlayerPos]++;
			}
			
			// reset deck for next iteration
			deck = new ArrayList<Card> (temp);
		}
		
		// End Main Loop
		
		
		
		// All empty hand values should be the same
		
		// Average win Array
		int total = 0;
		int emptyHands = 0;
		int[] emptyPositions = new int[this.players.length];
		int i = 0;
		for (Player player: this.players){
			if (player.hand[0] == null){
				total += winArray[player.position];
				emptyHands++;
				emptyPositions[i] = player.position;
				i++;
			}
		}
		double newValue = total*1.0/emptyHands;
		for (int j = 0; j < emptyHands; j++){		
			winArray[emptyPositions[j]] = newValue;
		}
		
		
		
		// Average tie Array
		total = 0;
		
		for (Player player: this.players){
			if (player.hand[0] == null){
				total += tieArray[player.position];
				
			}
		}
		newValue = total*1.0/emptyHands;
		for (int j = 0; j < emptyHands; j++){		
			tieArray[emptyPositions[j]] = newValue;
		}
		
		
		// Convert HashMap values to percentages
		DecimalFormat df2 = new DecimalFormat(".####");
		for (int j = 0; j < winArray.length; j++){
			winArray[j] = Double.valueOf(df2.format(winArray[j]/(1.0*iterations)));
		}
		for (int j = 0; j < tieArray.length; j++){
			tieArray[j] = Double.valueOf(df2.format(tieArray[j]/(1.0*iterations)));
		}
		
		
//		System.out.println(Arrays.toString(winArray));
//		System.out.println(Arrays.toString(tieArray));

	}
	
	public static void main(String[] args){
		
		Card h1 = new Card(Symbol.ACE, Suit.DIAMONDS);
		Card h2 = new Card(Symbol.TWO, Suit.DIAMONDS);
		
		Card h3 = new Card(Symbol.TWO, Suit.HEARTS);
		Card h4 = new Card(Symbol.QUEEN, Suit.SPADES);
//		
//		Card h5 = new Card(Symbol.EIGHT, Suit.HEARTS);
//		Card h6 = new Card(Symbol.KING, Suit.HEARTS);
//		
//		Card h7 = new Card(Symbol.TEN, Suit.DIAMONDS);
//		Card h8 = new Card(Symbol.ACE, Suit.CLUBS);
		
		Card b1 = new Card(Symbol.ACE, Suit.CLUBS);
		Card b2 = new Card(Symbol.KING, Suit.SPADES);
		Card b3 = new Card(Symbol.QUEEN, Suit.SPADES);
		Card b4 = new Card(Symbol.FIVE, Suit.DIAMONDS);
		Card b5 = new Card(Symbol.FOUR, Suit.SPADES);
		
		
		Card[] board = new Card[0];
		
		Player p1 = new Player(0,new Card[] {h1, h2});
		Player p2 = new Player(1,new Card[] {h3, h4});
		Player p3 = new Player(2,new Card[] {null, null});
		Player p4 = new Player(3,new Card[] {null, null});
		Player p5 = new Player(4,new Card[] {null, null});
		Player p6 = new Player(5,new Card[] {null, null});
		Player p7 = new Player(6,new Card[] {null, null});
		Player p8 = new Player(7,new Card[] {null, null});
		Player p9 = new Player(8,new Card[] {null, null});
		
		Player[] players = new Player[] {p1, p2, p3, p4, p5, p6, p7, p8, p9};
		
		HoldEm game = new HoldEm(players, board);
		
		double[] winArray = new double[players.length];
		double[] tieArray = new double[players.length];		
		
		
		System.out.println(game);
		long start = System.nanoTime();
		game.getWinPercentages(winArray, tieArray);
		long stop = System.nanoTime();
		
		System.out.println("Execution time: "+(stop-start)/(1000000000.0)+" seconds");
		
		//---------------------------------------------------------------------------------------
		
	}
}
