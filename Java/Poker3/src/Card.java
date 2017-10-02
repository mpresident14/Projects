
public class Card {
	int value;
	Suit suit;
	Symbol symbol;
	
	public Card(Symbol inputSymbol, Suit inputSuit){
		this.symbol = inputSymbol;
		this.suit = inputSuit;
		this.value = inputSymbol.number;
	}
	
	public String toString(){
		return this.symbol+" "+this.suit;
	}
	
	
	public int hashCode(){
		return this.value;
	}
	
	
	public boolean equals(Object obj){
		Card otherCard = (Card) obj;
		if (this.symbol.equals(otherCard.symbol) && 
				this.suit == otherCard.suit){
			
			return true;	
		}
		return false;
	}
	
	
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub

	}

}
